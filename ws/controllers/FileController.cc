#include "FileController.h"
#include <drogon/drogon.h>
#include <thread>
#include <iostream>
#include "helpers/CCSDSPacketFileHelper.h"
#include "helpers/UIDGeneratorHelper.h"
#include "helpers/EnvHelper.h"
#include "helpers/Constants.h"
#include "helpers/ControllerErrorHelper.h"

// this class is for working with file that is uploaded to the server for processing
// ambaqinejad
void FileController::uploadFile(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("Must only be one file");
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
    string documentRoot = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                                     "/home/ambaqinejad/Desktop/drogon_ccsds/ccsds_final_project/ws/public");
    string uploadPath = documentRoot + "/uploads";
    string filePath = uploadPath + "/" + file.getFileName();
    msg["fileUUID"] = fileUUID;
    msg["message"] = "The uploaded file has been saved";
    auto resp = HttpResponse::newHttpJsonResponse(msg);
    thread([filePath, fileUUID]() { CCSDSPacketFileHelper::processFile(filePath, fileUUID); }).detach();
    LOG_INFO << "The uploaded file has been saved to the " + uploadPath;
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
        string documentRoot = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                                                         Constants::DEFAULT_DOCUMENT_ROOT);
        string tempDir = documentRoot + Constants::DEFAULT_UPLOAD_DIR + "/" + fileUUID;
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
                                         "Error in starting upload.");
    }
}

std::mutex FileController::sessionsMutex_;
void FileController::uploadChunk(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    try {
        MultiPartParser fileUpload;
        if (fileUpload.parse(req) != 0) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("Invalid upload data");
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        auto params = fileUpload.getParameters();

        if (fileUpload.getFiles().size() != 1) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("No file chunk found");
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
        resp->setBody("Upload session not found");
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
        string documentRoot = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                                                         Constants::DEFAULT_DOCUMENT_ROOT);
        string finalPath = documentRoot + "/uploads/" + fileUUID + ".bin";
        std::ofstream outputFile(finalPath, std::ios::binary);
        if (!outputFile) {
            LOG_ERROR << "Failed to create output file: " << finalPath;
            return;
        }
        // Combine all chunks
        for (int i = 0; i < totalChunks; ++i) {
            std::string chunkPath = fileDir + "/chunk_" + std::to_string(i);
            std::ifstream chunkFile(chunkPath, std::ios::binary);

            if (!chunkFile) {
                LOG_ERROR << "Failed to read chunk: " << chunkPath;
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
    response["message"] = "File upload completed and processing started";

    auto resp = HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}


