#include "FileController.h"

#include <fstream>
#include <drogon/drogon.h>
#include <thread>

#include "database/MongoDBHandler.h"
#include "helpers/CCSDSPacketFileHelper.h"
#include "helpers/UIDGeneratorHelper.h"
#include "helpers/EnvHelper.h"
#include "helpers/Constants.h"
#include "helpers/ControllerErrorHelper.h"
#include "helpers/WorkingWithFileSystem.h"

// this class is for working with file that is uploaded to the server for processing
// ambaqinejad
void FileController::uploadFile(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    if (MongoDBHandler::ccsds_structure_.empty()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, Constants::STRUCTURE_NOT_FOUND);
    }
    MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(Constants::MUST_ONLY_BE_ONE_FILE);
        resp->setStatusCode(k403Forbidden);
        callback(resp);
        return;
    }

    HttpFile file = fileUpload.getFiles()[0];
    std::string fileUUID = UIDGeneratorHelper::generateUID();
    std::string fileName = fileUUID + "." + std::string(file.getFileExtension());
    auto md5 = file.getMd5();
    Json::Value msg;
    file.setFileName(fileName);
    file.save();
    string uploadPath = EnvHelper::readEnvVariable("UPLOAD_DIR",
                                     Constants::DEFAULT_UPLOAD_DIR);
    string filePath = uploadPath + "/" + file.getFileName();
    msg["fileUUID"] = fileUUID;
    msg["message"] = Constants::UPLOADED_FILE_HAVE_BEEN_SAVED;
    auto resp = HttpResponse::newHttpJsonResponse(msg);
    thread([filePath, fileUUID]() { CCSDSPacketFileHelper::processFile(filePath, fileUUID); }).detach();
    LOG_INFO << Constants::UPLOADED_FILE_HAVE_BEEN_SAVED_TO_THE + uploadPath;
    callback(resp);
}

std::unordered_map<std::string, std::string> FileController::fileUUIDToCorrespondingDirPath_;
std::unordered_map<std::string, int> FileController::fileUUIDToCorrespondingTotalChunks_;
void FileController::startUpload(const HttpRequestPtr &req,
                                                 std::function<void(const HttpResponsePtr &)> &&callback) {
    try {
        Json::Value jsonBody = req->getJsonObject() ? *req->getJsonObject() : Json::Value();
        auto fileName = jsonBody["fileName"].asString();
        auto totalChunks = jsonBody["totalChunks"].asString();
        std::string fileUUID = UIDGeneratorHelper::generateUID();
        string uploadPath = EnvHelper::readEnvVariable("UPLOAD_DIR",
                                                         Constants::DEFAULT_UPLOAD_DIR);
        string tempDir = uploadPath + "/" + fileUUID;
        std::filesystem::create_directories(tempDir);
        fileUUIDToCorrespondingDirPath_[fileUUID] = tempDir;
        fileUUIDToCorrespondingTotalChunks_[fileUUID] = stoi(totalChunks);
        Json::Value pktJson;
        pktJson["fileUUID"] = fileUUID;
        auto resp = HttpResponse::newHttpJsonResponse(pktJson);
        callback(resp);
    }
    catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k500InternalServerError,
                                         Constants::ERROR_IN_STARTING_UPLOAD);
    }
}

std::mutex FileController::sessionsMutex_;
void FileController::uploadChunk(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    try {
        MultiPartParser fileUpload;
        if (fileUpload.parse(req) != 0) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(Constants::INVALID_UPLOAD_DATA);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        auto params = fileUpload.getParameters();

        if (fileUpload.getFiles().size() != 1) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(Constants::NO_FILE_CHUNK_FOUND);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // Get chunk parameters
        std::string fileUUID = params["fileUUID"];
        int chunkIndex = std::stoi(params["chunkIndex"]);

        HttpFile file = fileUpload.getFiles()[0];

        std::unique_lock<std::mutex> lock(sessionsMutex_);

        std::string tempDir = fileUUIDToCorrespondingDirPath_[fileUUID];

        // Save chunk
        std::string chunkPath = tempDir + "/chunk_" + std::to_string(chunkIndex);
        file.saveAs(chunkPath);
        lock.unlock();

        Json::Value response;
        response["status"] = "success";
        response["chunkIndex"] = chunkIndex;
        response["fileUUID"] = fileUUID;
        if (FileController::fileUUIDToCorrespondingTotalChunks_[fileUUID]-1 == chunkIndex) {
            response["isCompleted"] = true;
        }
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    } catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k500InternalServerError,
                                         e.what());
    }
}

void FileController::finalizeUpload(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value jsonBody = req->getJsonObject() ? *req->getJsonObject() : Json::Value();

    std::string fileUUID = jsonBody["fileUUID"].asString();

    std::unique_lock<std::mutex> lock(sessionsMutex_);

    if (fileUUIDToCorrespondingDirPath_.find(fileUUID) == fileUUIDToCorrespondingDirPath_.end()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(Constants::UPLOAD_SESSION_NOT_FOUND);
        resp->setStatusCode(k404NotFound);
        callback(resp);
        return;
    }

    std::string fileDir = fileUUIDToCorrespondingDirPath_[fileUUID];
    int totalChunks = fileUUIDToCorrespondingTotalChunks_[fileUUID];

    // Remove from sessions
    fileUUIDToCorrespondingDirPath_.erase(fileUUID);
    fileUUIDToCorrespondingTotalChunks_.erase(fileUUID);
    lock.unlock();

    // Reassemble file in background thread
    std::thread([fileUUID, fileDir, totalChunks]() {
        string uploadPath = EnvHelper::readEnvVariable("UPLOAD_DIR",
                                                         Constants::DEFAULT_UPLOAD_DIR);
        string finalPath = uploadPath + "/" + fileUUID + ".bin";
        std::ofstream outputFile(finalPath, std::ios::binary);
        if (!outputFile) {
            LOG_ERROR << Constants::FAILED_TO_CREATE_OUTPUT_FILE << finalPath;
            return;
        }
        // Combine all chunks
        for (int i = 0; i < totalChunks; ++i) {
            std::string chunkPath = fileDir + "/chunk_" + std::to_string(i);
            std::ifstream chunkFile(chunkPath, std::ios::binary);

            if (!chunkFile) {
                LOG_ERROR << Constants::FAILED_TO_READ_CHUNK << chunkPath;
                outputFile.close();
                std::filesystem::remove(finalPath);
                return;
            }

            outputFile << chunkFile.rdbuf();
            chunkFile.close();

            // Delete chunk file
            std::filesystem::remove(chunkPath);
        }
        outputFile.close();

        // Remove temp directory
        std::filesystem::remove_all(fileDir);

        // Process complete file
        CCSDSPacketFileHelper::processFile(finalPath, fileUUID);
        LOG_INFO << "File processed: " << finalPath;
    }).detach();

    Json::Value response;
    response["fileUUID"] = fileUUID;
    response["message"] = Constants::FILE_UPLOAD_COMPLETED_AND_PROCESSING_STARTED;

    auto resp = HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void FileController::deleteUploadedFile(const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value jsonBody = req->getJsonObject() ? *req->getJsonObject() : Json::Value();
    std::string fileUUID = jsonBody["fileUUID"].asString();
    if (fileUUID == "") {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(Constants::REQUIRED_FIELDS);
        resp->setStatusCode(k404NotFound);
        callback(resp);
        return;
    }
    string uploadPath = EnvHelper::readEnvVariable("UPLOAD_DIR",
                          Constants::DEFAULT_UPLOAD_DIR);
    WorkingWithFileSystem::deleteFile(uploadPath, fileUUID+".bin");
    Json::Value response;
    response["fileUUID"] = fileUUID;
    response["message"] = Constants::FILE_DELETED;
    auto resp = HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}


