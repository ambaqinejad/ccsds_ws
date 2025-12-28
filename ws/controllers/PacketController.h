#pragma once

#include <drogon/HttpController.h>

#include "helpers/Constants.h"

using namespace drogon;

class PacketController : public drogon::HttpController<PacketController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PacketController::getSIDPacketsByPagination, Constants::GET_SID_PACKETS_BY_PAGINATION, Get);
    ADD_METHOD_TO(PacketController::persistAllPacketsInMongoDB, Constants::PERSIST_ALL_PACKETS_IN_MONGO_DB, Post);
    ADD_METHOD_TO(PacketController::persistAllPacketsInMongoDBBasedOnSID, Constants::PERSIST_ALL_PACKETS_IN_MONGO_DB_BASED_ON_SID, Post);
    ADD_METHOD_TO(PacketController::persistAllPacketsInCSVFile, Constants::PERSIST_ALL_PACKETS_IN_CSV_FILE, Post);
    ADD_METHOD_TO(PacketController::persistAllPacketsInCSVFileBasedOnSID, Constants::PERSIST_ALL_PACKETS_IN_CSV_FILE_BASED_ON_SID, Post);
    ADD_METHOD_TO(PacketController::downloadCSVFile, Constants::DOWNLOAD_CSV_FILE, Get);
    ADD_METHOD_TO(PacketController::getSidsList, "/getSidsList/{fileUUID}", Get);
    ADD_METHOD_TO(PacketController::updatePacketStructure, Constants::UPDATE_PACKET_STRUCTURE, Get);
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
