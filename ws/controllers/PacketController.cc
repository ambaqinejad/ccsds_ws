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
#include "helpers/Constants.h"

namespace fs = std::filesystem;

// Add definition of your processing function here
void PacketController::getSIDPacketsByPagination(const HttpRequestPtr &req,
                                                 std::function<void(const HttpResponsePtr &)> &&callback) {
    try {
        const auto sidStr = req->getParameter("sid");
        const auto pageStr = req->getParameter("page");
        const auto pageSizeStr = req->getParameter("pageSize");
        int _sid = 0;
        int _page = 0;
        int _pageSize = 0;

        _sid = std::stoi(sidStr);
        _page = std::stoi(pageStr);
        _pageSize = std::stoi(pageSizeStr); // Set your page size

        const std::string fileUUID = req->getParameter("fileUUID");

        // Get packets from UUID mapping
        const auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
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
        const int startIndex = (_page - 1) * _pageSize;
        const int endIndex = std::min(startIndex + _pageSize, static_cast<int>(filteredPackets.size()));

        if (startIndex >= static_cast<int>(filteredPackets.size())) {
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "Page out of range.");
        }

        const std::vector paginatedPackets(filteredPackets.begin() + startIndex,
                                           filteredPackets.begin() + endIndex);

        // Convert to JSON
        Json::Value pktJson;
        pktJson["num of all packets"] = static_cast<int>(allPackets.size());
        pktJson["num of all packets with this sid"] = static_cast<int>(filteredPackets.size());
        pktJson["total pages"] =
                filteredPackets.size() % _pageSize != 0
                    ? filteredPackets.size() / _pageSize + 1
                    : filteredPackets.size() /
                      _pageSize;
        pktJson["sid"] = _sid;
        Json::Value resultJson(Json::arrayValue);
        for (const auto &pkt: paginatedPackets) {
            // or whatever your serialization method is
            resultJson.append(pkt.toJson());
        }
        pktJson["packets"] = resultJson;
        const auto resp = HttpResponse::newHttpJsonResponse(pktJson);
        callback(resp);
    } catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         e.what());
    }
}

void PacketController::persistAllPacketsInMongoDB(const HttpRequestPtr &req,
                                                  function<void(const HttpResponsePtr &)> &&callback) {
    try {
        const std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();

        // Lock to prevent race conditions
        static std::mutex mapperMutex;
        std::unique_lock lock(mapperMutex);

        const auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
        if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
        }

        // MOVE the data instead of copying - this is nearly instantaneous
        auto packets = std::make_shared<std::vector<CCSDS_Packet> >(it->second); // copy

        // Remove from the map immediately to free memory
        lock.unlock(); // Release the lock
        LOG_INFO << "Starting MongoDB persistence for " << packets->size() << " packets";
        thread([packets]() {
            try {
                const int eachTimeNotifyClients = static_cast<int>(packets->size()) /
                                                  ClientCommunicationHelper::progressDivider != 0
                                                      ? static_cast<int>(packets->size()) /
                                                        ClientCommunicationHelper::progressDivider
                                                      : 1;
                for (size_t i = 0; i < packets->size(); ++i) {
                    MongoDBHandler dbHandler;
                    auto packet = packets->at(i);
                    dbHandler.insertPacket(packet);
                    if (i % eachTimeNotifyClients == 0) {
                        const int progress = std::ceil(
                            (static_cast<double>(i + 1) / static_cast<double>(packets->size())) * 100);
                        ClientCommunicationHelper::notifyClients(progress);
                    }
                }
            } catch (const std::exception &e) {
                LOG_ERROR << Constants::ERROR_IN_MONGO_DB_PERSISTENCE << e.what();
                ClientCommunicationHelper::notifyClients(-1);
            }
        }).detach();

        Json::Value pktJson;
        pktJson["message"] = Constants::PACKET_INSERTION_IS_IN_PROGRESS;
        pktJson["packetCount"] = static_cast<Json::UInt64>(packets->size());
        const auto resp = HttpResponse::newHttpJsonResponse(pktJson);
        callback(resp);
    } catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         e.what());
    }
}

void PacketController::persistAllPacketsInMongoDBBasedOnSID(const HttpRequestPtr &req,
                                                            function<void(const HttpResponsePtr &)> &&callback) {
    try {
        const auto sidStr = (*req->getJsonObject())["sid"].asString();
        int _sid = 0;
        _sid = std::stoi(sidStr);
        const std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();
        const auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
        if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, Constants::FILE_UUID_NOT_FOUND);
        }

        const std::vector<CCSDS_Packet> &allPackets = it->second;
        std::vector<CCSDS_Packet> filteredPackets;
        for (const auto &pkt: allPackets) {
            if (pkt.sid == _sid) // assuming CCSDS_Packet has a `sid` field
            {
                filteredPackets.push_back(pkt);
            }
        }
        auto packetsCopy = std::make_shared<std::vector<CCSDS_Packet> >(filteredPackets);
        thread([packetsCopy]() {
            try {
                const int eachTimeNotifyClients = static_cast<int>(packetsCopy->size()) /
                                                  ClientCommunicationHelper::progressDivider != 0
                                                      ? static_cast<int>(packetsCopy->size()) /
                                                        ClientCommunicationHelper::progressDivider
                                                      : 1;

                for (size_t i = 0; i < packetsCopy->size(); ++i) {
                    MongoDBHandler dbHandler;
                    auto packet = packetsCopy->at(i);
                    dbHandler.insertPacket(packet);
                    if (i % eachTimeNotifyClients == 0) {
                        const int progress = std::ceil(
                            (static_cast<double>(i + 1) / static_cast<double>(packetsCopy->size())) * 100);
                        ClientCommunicationHelper::notifyClients(progress);
                    }
                }
            } catch (const exception &e) {
                ClientCommunicationHelper::notifyClients(-1);
                LOG_ERROR << "Exception caught: " << e.what() << "\n";
            }
        }).detach();
        Json::Value pktJson;
        pktJson["message"] = Constants::PACKET_INSERTION_IS_IN_PROGRESS;
        auto resp = HttpResponse::newHttpJsonResponse(pktJson);
        callback(resp);
    } catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         e.what());
    }
}

void PacketController::persistAllPacketsInCSVFile(const HttpRequestPtr &req,
                                                  function<void(const HttpResponsePtr &)> &&callback) {
    try {
        std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();
        const auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
        if (it == CCSDSPacketFileHelper::uuidToSavedPacketsMapper.end()) {
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, "File UUID not found.");
        }
        const std::vector<CCSDS_Packet> allPackets = it->second;
        auto packetsCopy = std::make_shared<std::vector<CCSDS_Packet> >(allPackets);
        thread([packetsCopy, fileUUID]() {
            try {
                const int eachTimeNotifyClients = static_cast<int>(packetsCopy->size()) /
                                                  ClientCommunicationHelper::progressDivider != 0
                                                      ? static_cast<int>(packetsCopy->size()) /
                                                        ClientCommunicationHelper::progressDivider
                                                      : 1;

                for (size_t i = 0; i < packetsCopy->size(); ++i) {
                    auto packet = packetsCopy->at(i);
                    const bool isSuccessful = CSVHandler::insertPacket(packet, fileUUID);
                    if (i % eachTimeNotifyClients == 0) {
                        const int progress = std::ceil(
                            (static_cast<double>(i + 1) / static_cast<double>(packetsCopy->size())) * 100);
                        ClientCommunicationHelper::notifyClients(isSuccessful ? progress : -1);
                    }
                }
            } catch (const exception &e) {
                ClientCommunicationHelper::notifyClients(-1);
                LOG_ERROR << "Exception caught: " << e.what() << "\n";
            }
        }).detach();
        Json::Value pktJson;
        pktJson["message"] = Constants::PACKET_INSERTION_IS_IN_PROGRESS;
        const auto resp = HttpResponse::newHttpJsonResponse(pktJson);
        callback(resp);
    } catch (const std::exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         e.what());
    }
}

void
PacketController::downloadCSVFile(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    try {
        string sid = req->getParameter("sid");
        string fileUUID = req->getParameter("fileUUID");
        string csvPath = EnvHelper::readEnvVariable("CSV_DIR",
                                                    Constants::DEFAULT_CSV_DIR);
        std::string filename = "SID" + sid + ".csv";
        std::string directoryPath = csvPath + "/" + fileUUID;
        if (!fs::exists(directoryPath)) {
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, Constants::FILE_NOT_FOUND);
        }
        std::string filePath = directoryPath + "/" + filename;
        // Open file in append mode
        if (std::ofstream csvFile(filePath, std::ios::app); !csvFile.is_open()) {
            LOG_ERROR << Constants::FAILED_TO_OPEN_CSV_FILE;
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound,
                                                    Constants::FAILED_TO_OPEN_CSV_FILE);
        }
        auto resp = HttpResponse::newFileResponse(filePath, filename);
        callback(resp);
    } catch (const exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         e.what());
    }
}

void
PacketController::getSidsList(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback,
                              const string &fileUUID) {
    try {
        if (MongoDBHandler::ccsds_structure_.empty()) {
            return ControllerErrorHelper::sendError(std::move(callback), k404NotFound, Constants::STRUCTURE_NOT_FOUND);
        }
        const auto it = CCSDSPacketFileHelper::uuidToSids.find(fileUUID);
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
            auto iterator = std::find_if(ccsds_structure.begin(), ccsds_structure.end(), [&](const auto &_obj) {
                return _obj.contains("metadata")
                       && _obj["metadata"].contains("SIDNumber")
                       && _obj["metadata"]["SIDNumber"] == sid;
            });

            if (iterator != ccsds_structure.end()) {
                if (auto &entry = *iterator; entry.contains("metadata") &&
                                             entry["metadata"].contains("SID")) {
                    const std::string sid_name = entry["metadata"]["SID"];
                    obj["sid"] = sid; // numeric
                    obj["sid_name"] = sid_name; // your lookup method
                    sidArray.append(obj);
                }
                // use foundObj
            }
        }
        sidJson["sids"] = sidArray;
        const auto resp = HttpResponse::newHttpJsonResponse(sidJson);
        callback(resp);
    } catch (const exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest,
                                         e.what());
    }
}

void PacketController::persistAllPacketsInCSVFileBasedOnSID(const HttpRequestPtr &req,
                                                            function<void(const HttpResponsePtr &)> &&callback) {
    try {
        const auto sidStr = (*req->getJsonObject())["sid"].asString();
        int _sid = 0;
        _sid = std::stoi(sidStr);

        std::string fileUUID = (*req->getJsonObject())["fileUUID"].asString();
        const auto it = CCSDSPacketFileHelper::uuidToSavedPacketsMapper.find(fileUUID);
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
        auto packetsCopy = std::make_shared<std::vector<CCSDS_Packet> >(filteredPackets);
        thread([packetsCopy, fileUUID]() {
            try {
                const int eachTimeNotifyClients = static_cast<int>(packetsCopy->size()) / ClientCommunicationHelper::progressDivider != 0
                                                ? static_cast<int>(packetsCopy->size()) / ClientCommunicationHelper::progressDivider
                                                : 1;

                for (size_t i = 0; i < packetsCopy->size(); ++i) {
                    auto packet = packetsCopy->at(i);
                    const bool isSuccessful = CSVHandler::insertPacket(packet, fileUUID);
                    if (i % eachTimeNotifyClients == 0) {
                        const int progress = std::ceil((static_cast<double>(i + 1) / static_cast<double>(packetsCopy->size())) * 100);
                        ClientCommunicationHelper::notifyClients(isSuccessful ? progress : -1);
                    }
                }
            } catch (const exception &e) {
                ClientCommunicationHelper::notifyClients(-1);
                LOG_ERROR << "Exception caught: " << e.what() << "\n";
            }
        }).detach();
        Json::Value pktJson;
        pktJson["message"] = Constants::PACKETS_INSERTING_WEB_SOCKET;
        pktJson["link"] = req->getLocalAddr().toIpPort() + "/downloadCSVFile?fileUUID=" + fileUUID + "&sid=" + sidStr;
        const auto resp = HttpResponse::newHttpJsonResponse(pktJson);
        callback(resp);
    } catch (exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest, e.what());
    }
}

void
PacketController::updatePacketStructure(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    try {
        Json::Value resultJson;
        if (MongoDBHandler dbHandler; !dbHandler.loadStructure()) {
            LOG_INFO << Constants::STRUCTURE_COULD_NOT_LOAD_FROM_DB_TO_RAM;
            resultJson["message"] = Constants::PACKETS_INSERTING_WEB_SOCKET;
            const auto resp = HttpResponse::newHttpJsonResponse(resultJson);
            return callback(resp);
        }
        resultJson["message"] = Constants::STRUCTURE_UPDATED_SUCCESSFULLY;
        const auto resp = HttpResponse::newHttpJsonResponse(resultJson);
        callback(resp);
    } catch (const exception &e) {
        ControllerErrorHelper::sendError(std::move(callback), k400BadRequest, e.what());
    }
}
