#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <fstream>
#include <thread>
#include <chrono>

#include "logics/CCSDS_Packet.h"

using namespace drogon;
using namespace std;

class FileController : public drogon::HttpController<FileController>
{
  // this class is for working with file that is uploaded to the server for processing
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FileController::uploadFile, "/upload", Post);
    ADD_METHOD_TO(FileController::startUpload, "/startUpload", Post);
    ADD_METHOD_TO(FileController::uploadChunk, "/uploadChunk", Post);
    ADD_METHOD_TO(FileController::finalizeUpload, "/finalizeUpload", Post);
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
 
