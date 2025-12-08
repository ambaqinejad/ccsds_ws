#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>

#include "logics/CCSDS_Packet.h"
#include "helpers/Constants.h"

using namespace drogon;
using namespace std;

class FileController : public drogon::HttpController<FileController>
{
  // this class is for working with file that is uploaded to the server for processing
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FileController::uploadFile, Constants::FILE_UPLOAD, Post);
    ADD_METHOD_TO(FileController::startUpload, Constants::FILE_START_UPLOAD, Post);
    ADD_METHOD_TO(FileController::uploadChunk, Constants::FILE_UPLOAD_CHUNK, Post);
    ADD_METHOD_TO(FileController::finalizeUpload, Constants::FILE_FINALIZE_UPLOAD, Post);
    METHOD_LIST_END
    static void uploadFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void startUpload(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void uploadChunk(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void finalizeUpload(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
private:
    static std::mutex sessionsMutex_;
    static std::unordered_map<std::string, std::string> fileUUIDToCorrespondingDirPath_;
    static std::unordered_map<std::string, int> fileUUIDToCorrespondingTotalChunks_;
};
 
