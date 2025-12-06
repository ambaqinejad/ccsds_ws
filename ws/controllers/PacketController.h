#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PacketController : public drogon::HttpController<PacketController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PacketController::getSIDPacketsByPagination, "/getSIDPacketsByPagination", Get);
    ADD_METHOD_TO(PacketController::persistAllPacketsInMongoDB, "/persistAllPacketsInMongoDB", Post);
    ADD_METHOD_TO(PacketController::persistAllPacketsInMongoDBBasedOnSID, "/persistAllPacketsInMongoDBBasedOnSID", Post);
    ADD_METHOD_TO(PacketController::persistAllPacketsInCSVFile, "/persistAllPacketsInCSVFile", Post);
    ADD_METHOD_TO(PacketController::persistAllPacketsInCSVFileBasedOnSID, "/persistAllPacketsInCSVFileBasedOnSID", Post);
    ADD_METHOD_TO(PacketController::downloadCSVFile, "/downloadCSVFile", Get);
    ADD_METHOD_TO(PacketController::getSidsList, "/getSidsList/{fileUUID}", Get);
    ADD_METHOD_TO(PacketController::updatePacketStructure, "/updatePacketStructure", Get);


    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    static void getSIDPacketsByPagination(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void persistAllPacketsInMongoDB(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void persistAllPacketsInMongoDBBasedOnSID(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void persistAllPacketsInCSVFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void persistAllPacketsInCSVFileBasedOnSID(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void downloadCSVFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) ;
    static void getSidsList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string& fileUUID);
    static void updatePacketStructure(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
