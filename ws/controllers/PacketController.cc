#include <fstream>
#include "PacketController.h"
#include "helpers/ControllerErrorHelper.h"
#include "logics/CCSDS_Packet.h"
#include "helpers/CCSDSPacketFileHelper.h"
#include "database/MongoDBHandler.h"
#include "database/CSVHandler.h"
#include "helpers/ClientCommunicationHelper.h"
#include <filesystem>
#include "helpers/EnvHelper.h"

namespace fs = std::filesystem;

// Add definition of your processing function here
void PacketController::getSIDPacketsByPagination(const HttpRequestPtr &req,
                                                 std::function<void(const HttpResponsePtr &)> &&callback) {
    auto sidStr = req->getParameter("sid");
    auto pageStr = req->getParameter("page");
    auto pageSizeStr = req->getParameter("pageSize");

    int _sid = 0;
    int _page = 0;
    int _pageSize = 0;

    try {
        _sid = std::stoi(sidStr);
        _page = std::stoi(pageStr);
        _pageSize = std::stoi(pageSizeStr); // Set your page size
    }
    catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         "Invalid sid or page or pageSize parameter.");
    }
    std::string fileUUID = req->getParameter("fileUUID");

    // Get packets from UUID mapping
    auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
    if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
    }

    const std::vector<CCSDS_Packet> &allPackets = it->second;

    // Filter by SID
    std::vector<CCSDS_Packet> filteredPackets;
    for (const auto &pkt: allPackets) {
        if (pkt.sid == _sid) // assuming CCSDS_Packet has a `sid` field
        {
            filteredPackets.push_back(pkt);
        }
    }

    // Paginate
    int startIndex = (_page - 1) * _pageSize;
    int endIndex = std::min(startIndex + _pageSize, static_cast<int>(filteredPackets.size()));

    if (startIndex >= static_cast<int>(filteredPackets.size())) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "Page out of range.");
    }

    std::vector<CCSDS_Packet> paginatedPackets(filteredPackets.begin() + startIndex,
                                               filteredPackets.begin() + endIndex);

    // Convert to JSON
    Json::Value pktJson;
    pktJson["num of all packets"] = static_cast<int>(allPackets.size());
    pktJson["num of all packets with this sid"] = static_cast<int>(filteredPackets.size());
    pktJson["total pages"] =
            filteredPackets.size() % _pageSize != 0 ? filteredPackets.size() / _pageSize + 1 : filteredPackets.size() /
                                                                                               _pageSize;
    pktJson["sid"] = _sid;
    Json::Value resultJson(Json::arrayValue);
    for (const auto &pkt: paginatedPackets) {
        // or whatever your serialization method is
        resultJson.append(pkt.toJson());
    }
    pktJson["packets"] = resultJson;
    auto resp = HttpResponse::newHttpJsonResponse(pktJson);
    callback(resp);

}

void PacketController::persistAllPacketsInMongoDB(const HttpRequestPtr &req,
                                                  function<void(const HttpResponsePtr &)> &&callback) {

    std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();

    // Lock to prevent race conditions
    static std::mutex mapperMutex;
    std::unique_lock<std::mutex> lock(mapperMutex);

    auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
    if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
    }

    // MOVE the data instead of copying - this is nearly instantaneous
    auto packets = std::make_shared<std::vector<CCSDS_Packet>>(std::move(it->second));

    // Remove from the map immediately to free memory
    CCSDSPacketFileHelper::uuidToSavedPacketsMapper.erase(it);
    lock.unlock(); // Release the lock

    LOG_INFO << "Starting MongoDB persistence for " << packets->size() << " packets";

    thread([packets]() {
        try {
            MongoDBHandler dbHandler;
            int eachTimeNotifyClients = (int) packets->size() / ClientCommunicationHelper::progressDivider != 0 ?
                                        (int) packets->size() / ClientCommunicationHelper::progressDivider : 1;
            for (size_t i = 0; i < packets->size(); ++i) {
                auto packet = packets->at(i);
                dbHandler.insertPacket(packet);
                if (i % eachTimeNotifyClients == 0) {
                    int progress = std::ceil(((double) (i+1) / (double) packets->size()) * 100);
                    ClientCommunicationHelper::notifyClients(progress);
                }
            }

        } catch (const std::exception& e) {
            LOG_ERROR << "Error in MongoDB persistence: " << e.what();
            ClientCommunicationHelper::notifyClients(-1);
        }
    }).detach();

    Json::Value pktJson;
    pktJson["message"] = "Packets insertion is in progress.";
    pktJson["packetCount"] = static_cast<Json::UInt64>(packets->size());
    auto resp = HttpResponse::newHttpJsonResponse(pktJson);
    callback(resp);
}

void PacketController::persistAllPacketsInMongoDBBasedOnSID(const HttpRequestPtr &req,
                                                            function<void(const HttpResponsePtr &)> &&callback) {
    MongoDBHandler dbHandler;
    auto sidStr = (*req->getJsonObject())["sid"].asString();
    int _sid = 0;
    try {
        _sid = std::stoi(sidStr);
    }
    catch (const std::exception &e) {
        return ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                                "Invalid sid parameter.");
    }
    std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();
    auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
    if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
    }

    const std::vector<CCSDS_Packet> &allPackets = it->second;
    std::vector<CCSDS_Packet> filteredPackets;
    for (const auto &pkt: allPackets) {
        if (pkt.sid == _sid) // assuming CCSDS_Packet has a `sid` field
        {
            filteredPackets.push_back(pkt);
        }
    }
    auto packetsCopy = std::make_shared<std::vector<CCSDS_Packet>>(filteredPackets);
    thread([packetsCopy]() {
        MongoDBHandler dbHandler;
        int eachTimeNotifyClients = (int) packetsCopy->size() / ClientCommunicationHelper::progressDivider != 0 ?
                                    (int) packetsCopy->size() / ClientCommunicationHelper::progressDivider : 1;

        for (size_t i = 0; i < packetsCopy->size(); ++i) {
            auto packet = packetsCopy->at(i);
            dbHandler.insertPacket(packet);
            if (i % eachTimeNotifyClients == 0) {
                int progress = std::ceil(((double) (i+1) / (double) packetsCopy->size()) * 100);
                ClientCommunicationHelper::notifyClients(progress);
            }
        }
    }).detach();
    Json::Value pktJson;
    pktJson["message"] = "Packets insertion is in progress.";
    auto resp = HttpResponse::newHttpJsonResponse(pktJson);
    callback(resp);
}

void PacketController::persistAllPacketsInCSVFile(const HttpRequestPtr &req,
                                                  function<void(const HttpResponsePtr &)> &&callback) {
    std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();
    auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
    if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
    }
    const std::vector<CCSDS_Packet> allPackets = it->second;
    auto packetsCopy = std::make_shared<std::vector<CCSDS_Packet>>(allPackets);
    thread([packetsCopy, fileUUID]() {
        try {
            int eachTimeNotifyClients = (int) packetsCopy->size() / ClientCommunicationHelper::progressDivider != 0 ?
                                        (int) packetsCopy->size() / ClientCommunicationHelper::progressDivider : 1;

            for (size_t i = 0; i < packetsCopy->size(); ++i) {
                auto packet = packetsCopy->at(i);
                bool isSuccessful = CSVHandler::insertPacket(packet, fileUUID);
                if (i % eachTimeNotifyClients == 0) {
                    int progress = std::ceil(((double) (i+1) / (double) packetsCopy->size()) * 100);
                    ClientCommunicationHelper::notifyClients(isSuccessful ? progress : -1);
                }
            }
        } catch (const exception &e) {
            ClientCommunicationHelper::notifyClients(-1);
            LOG_ERROR << "Exception caught: " << e.what() << "\n";
        }

    }).detach();
    Json::Value pktJson;
    pktJson["message"] = "Packets insertion is in progress.";
    auto resp = HttpResponse::newHttpJsonResponse(pktJson);
    callback(resp);
}

void
PacketController::downloadCSVFile(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    string sid = req->getParameter("sid");
    string fileUUID = req->getParameter("fileUUID");
    string directoryBasePath = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                                          "/home/ambaqinejad/Desktop/drogon_ccsds/ccsds_final_project/ws/public");
    std::string filename = "SID" + sid + ".csv";
    std::string directoryPath = directoryBasePath + "/" + fileUUID;
    if (!fs::exists(directoryPath)) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File not found.");
    }
    std::string filePath = directoryPath + "/" + filename;
    // Open file in append mode
    std::ofstream csvFile(filePath, std::ios::app);
    if (!csvFile.is_open()) {
        LOG_ERROR << "Failed to open CSV file.";
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "Failed to open CSV file.");
    }
    auto resp = HttpResponse::newFileResponse(filePath, filename);
    callback(resp);
}

void
PacketController::getSidsList(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback,
                              const string &fileUUID) {
    auto it = CCSDSPacketFileHelper::uuidToSids.find(fileUUID);
    if (it == CCSDSPacketFileHelper::uuidToSids.end()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
    }

    const std::set<uint8_t> &allSids = it->second;

    // Convert to JSON
    Json::Value sidJson;
    sidJson["fileUUID"] = fileUUID;
    sidJson["numOfAllSids"] = static_cast<int>(allSids.size());
    Json::Value sidArray(Json::arrayValue);
    for (uint8_t sid: allSids) {
        Json::Value obj(Json::objectValue);
        auto &ccsds_structure = MongoDBHandler::ccsds_structure_;
        auto iterator = std::find_if(ccsds_structure.begin(), ccsds_structure.end(), [&](const auto& obj) {
            return obj.contains("metadata")
                   && obj["metadata"].contains("SIDNumber")
                   && obj["metadata"]["SIDNumber"] == sid;
        });

        if (iterator != ccsds_structure.end()) {
            auto &entry = *iterator;
            if (entry.contains("metadata") &&
                entry["metadata"].contains("SID")) {
                std::string sid_name = entry["metadata"]["SID"];
                obj["sid"] = sid;  // numeric
                obj["sid_name"] = sid_name;  // your lookup method
                sidArray.append(obj);
            }
            // use foundObj
        }
    }
    sidJson["sids"] = sidArray;
    auto resp = HttpResponse::newHttpJsonResponse(sidJson);
    callback(resp);
}

void PacketController::persistAllPacketsInCSVFileBasedOnSID(const HttpRequestPtr &req,
                                                            function<void(const HttpResponsePtr &)> &&callback) {
    auto sidStr = (*req->getJsonObject())["sid"].asString();
    int _sid = 0;
    try {
        _sid = std::stoi(sidStr);
    }
    catch (const std::exception &e) {
        return ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                                "Invalid sid parameter.");
    }
    std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();
    auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
    if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
        return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
    }

    const std::vector<CCSDS_Packet> &allPackets = it->second;
    std::vector<CCSDS_Packet> filteredPackets;
    for (const auto &pkt: allPackets) {
        if (pkt.sid == _sid) // assuming CCSDS_Packet has a `sid` field
        {
            filteredPackets.push_back(pkt);
        }
    }
    auto packetsCopy = std::make_shared<std::vector<CCSDS_Packet>>(filteredPackets);
    thread([packetsCopy, fileUUID]() {
        try {
            int eachTimeNotifyClients = (int) packetsCopy->size() / ClientCommunicationHelper::progressDivider != 0 ?
                                        (int) packetsCopy->size() / ClientCommunicationHelper::progressDivider : 1;

            for (size_t i = 0; i < packetsCopy->size(); ++i) {
                auto packet = packetsCopy->at(i);
                bool isSuccessful = CSVHandler::insertPacket(packet, fileUUID);
                if (i % eachTimeNotifyClients == 0) {
                    int progress = std::ceil(((double) (i+1) / (double) packetsCopy->size()) * 100);
                    ClientCommunicationHelper::notifyClients(isSuccessful ? progress : -1);
                }
            }
        } catch (const exception &e) {
            ClientCommunicationHelper::notifyClients(-1);
            LOG_ERROR << "Exception caught: " << e.what() << "\n";
        }

    }).detach();
    Json::Value pktJson;
    pktJson["message"] = "Packets are inserting. Check web socket.";
    pktJson["link"] = req->getLocalAddr().toIpPort() + "/downloadCSVFile?fileUUID=" + fileUUID + "&sid=" + sidStr;
    auto resp = HttpResponse::newHttpJsonResponse(pktJson);
    callback(resp);
}

void
PacketController::updatePacketStructure(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    MongoDBHandler dbHandler;
    Json::Value resultJson;
    if (!dbHandler.loadStructure()) {
        LOG_INFO << "Structure did not load and can not be updated.";
        resultJson["message"] = "Packets are inserting. Check web socket.";
        auto resp = HttpResponse::newHttpJsonResponse(resultJson);
        return callback(resp);
    }
    resultJson["message"] = "Structure updated successfully.";
    auto resp = HttpResponse::newHttpJsonResponse(resultJson);
    callback(resp);
}
