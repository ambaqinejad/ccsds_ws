//
// Created by ambaqinejad on 3/27/25.
//

#include "MongoDBHandler.h"

#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>
#include <json/json.h>
#include "logics/CCSDS_Packet.h"
#include <drogon/HttpController.h>
#include "helpers/EnvHelper.h"
#include "helpers/Constants.h"
#include <chrono>
using namespace std::chrono;

MongoDBHandler::MongoDBHandler() {
    static mongocxx::instance instance{}; // Required once per application
    std::string uri = EnvHelper::readEnvVariable("MONGODB_URI",
                                 Constants::MONGODB_DEFAULT_URI);
    LOG_INFO << "DATABASE URI -------> " << uri;
    client_ = mongocxx::client{mongocxx::uri{uri}};
    database_ = client_[Constants::DEFAULT_DB_NAME];
}



bool MongoDBHandler::insertPacket(const CCSDS_Packet &packet) {
    // Serialize the extended_payload
    try {
        auto start = high_resolution_clock::now();
        mongocxx::collection collection;
        std::string collection_name = "SID" + std::to_string(packet.sid);
        collection = database_[collection_name];
        bsoncxx::builder::basic::document doc{};
        insertHeader(doc, packet);

        Json::StreamWriterBuilder writer;
        std::string jsonStr = Json::writeString(writer, packet.parsedData);

        // Step 3: Parse JSON string into BSON
        bsoncxx::document::value subDocument = bsoncxx::from_json(jsonStr);
        doc.append(bsoncxx::builder::basic::kvp("data", subDocument));
        collection.insert_one(doc.view());
        auto end = high_resolution_clock::now();
        auto duration_us = duration_cast<microseconds>(end - start);  // microseconds
        LOG_INFO << "Packet inserted successfully. Time (microseconds): " << duration_us.count() << " µs";
        return true;
    } catch (const exception &e) {
        LOG_ERROR << "Exception caught: " << e.what() << "\n";
        return false;
    }
}

void MongoDBHandler::insertHeader(bsoncxx::builder::basic::document &document, const CCSDS_Packet &packet) {
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_MAIN_FRAME_HEADER, packet.main_frame_header));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_PACKET_ID, packet.packet_id));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_PACKET_SEQUENCE_CONTROL, packet.packet_sequence_control));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_PACKET_LENGTH, packet.packet_length));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_DATA_FIELD_HEADER, static_cast<int>(packet.data_field_header)));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_SERVICE_TYPE, static_cast<int>(packet.service_type)));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_SUB_SERVICE_TYPE, static_cast<int>(packet.sub_service_type)));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_SID, static_cast<int>(packet.sid)));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_TIMESTAMP, static_cast<int64_t>(packet.timestamp)));
    document.append(
            bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_CRC_FAIL_UPLOAD_MAP, static_cast<int64_t>(packet.crc_fail_upload_map)));
    document.append(bsoncxx::builder::basic::kvp(Constants::PACKET_HEADER_KEY_FLASH_ADDRESS, static_cast<int>(packet.flash_address)));
}

void MongoDBHandler::insertStructure(nlohmann::ordered_json json) {
    mongocxx::collection collection = database_[Constants::DEFAULT_COLLECTION_NAME];
    collection.drop(); // Clear previous data

    int i = 1;
    for (auto& item : json.items()) {
        nlohmann::ordered_json obj = item.value();

        // Set the _id field to the value of sid
        obj["_id"] = i;

        bsoncxx::document::value doc = bsoncxx::from_json(obj.dump());
        collection.insert_one(doc.view());
        i++;
    }
}

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
nlohmann::ordered_json MongoDBHandler::ccsds_structure_;
bool MongoDBHandler::loadStructure() {
    mongocxx::collection history_collection;
    history_collection = database_[Constants::DEFAULT_HISTORY_COLLECTION_NAME];
    auto filter = document{} << "is_current" << true << finalize;
    auto result = history_collection.find_one(filter.view());
    if (result && (*result)["collection_name"]) {
        LOG_INFO << "Name: " << (*result)["collection_name"].get_string().value;
        mongocxx::collection structure_collection = database_[(*result)["collection_name"].get_string().value];
        auto cursor = structure_collection.find({});
        MongoDBHandler::ccsds_structure_ = nlohmann::ordered_json::array();
        for (auto &&doc: cursor) {
            std::string json_str = bsoncxx::to_json(doc);
            nlohmann::ordered_json j = nlohmann::ordered_json::parse(json_str);
            MongoDBHandler::ccsds_structure_.push_back(j);
        }
        if (!MongoDBHandler::ccsds_structure_.empty()) {
            LOG_INFO << Constants::STRUCTURE_LOADED_FROM_DB_TO_RAM;
            return true;
        }
    }
    LOG_INFO << Constants::STRUCTURE_COULD_NOT_LOAD_FROM_DB_TO_RAM;
    return false;
}

bool MongoDBHandler::insertPacketsBulk(const vector<CCSDS_Packet> &packets, size_t batchSize) {
    return false;
//    try {
//        auto totalStart = high_resolution_clock::now();
//        size_t totalPackets = packets.size();
//        size_t processedPackets = 0;
//
//        LOG_INFO << "Starting bulk insertion of " << totalPackets << " packets with batch size " << batchSize;
//
//        // Group packets by SID (collection name)
//        std::unordered_map<std::string, std::vector<const CCSDS_Packet*>> packetsByCollection;
//        for (const auto& packet : packets) {
//            std::string collection_name = "SID" + std::to_string(packet.sid);
//            packetsByCollection[collection_name].push_back(&packet);
//        }
//
//        LOG_INFO << "Packets distributed across " << packetsByCollection.size() << " collections";
//
//        // Process each collection
//        for (const auto& [collection_name, collectionPackets] : packetsByCollection) {
//            LOG_INFO << "Processing collection: " << collection_name << " with " << collectionPackets.size() << " packets";
//
//            mongocxx::collection collection = database_[collection_name];
//            size_t collectionSize = collectionPackets.size();
//
//            // Process in batches
//            for (size_t startIdx = 0; startIdx < collectionSize; startIdx += batchSize) {
//                size_t endIdx = std::min(startIdx + batchSize, collectionSize);
//                size_t currentBatchSize = endIdx - startIdx;
//
//                auto batchStart = high_resolution_clock::now();
//
//                // Create bulk operation
//                mongocxx::bulk_write bulk = collection.create_bulk_write();
//
//                for (size_t i = startIdx; i < endIdx; i++) {
//                    const CCSDS_Packet* packet = collectionPackets[i];
//
//                    bsoncxx::builder::basic::document doc{};
//                    insertHeader(doc, *packet);
//
//                    // Convert parsedData to BSON
//                    Json::StreamWriterBuilder writer;
//                    std::string jsonStr = Json::writeString(writer, packet->parsedData);
//                    bsoncxx::document::value subDocument = bsoncxx::from_json(jsonStr);
//                    doc.append(bsoncxx::builder::basic::kvp("data", subDocument));
//
//                    // Add to bulk operation as insert
//                    mongocxx::model::insert_one insert_op{doc.view()};
//                    bulk.append(insert_op);
//                }
//
//                // Execute bulk operation
//                mongocxx::stdx::optional<mongocxx::result::bulk_write> result = bulk.execute();
//
//                auto batchEnd = high_resolution_clock::now();
//                auto batchDuration = duration_cast<milliseconds>(batchEnd - batchStart);
//
//                processedPackets += currentBatchSize;
//
//                // Progress reporting
//                int progress = std::ceil(((double)processedPackets / (double)totalPackets) * 100);
//                LOG_INFO << "Batch completed: " << currentBatchSize << " packets in "
//                         << batchDuration.count() << "ms. Total progress: " << progress << "%";
//
//                // Notify clients of progress
//                if (processedPackets % std::max(size_t(1), totalPackets / ClientCommunicationHelper::progressDivider) == 0) {
//                    ClientCommunicationHelper::notifyClients(progress);
//                }
//            }
//        }
//
//        auto totalEnd = high_resolution_clock::now();
//        auto totalDuration = duration_cast<seconds>(totalEnd - totalStart);
//
//        LOG_INFO << "Bulk insertion completed: " << totalPackets << " packets in "
//                 << totalDuration.count() << " seconds";
//
//        return true;
//
//    } catch (const std::exception& e) {
//        LOG_ERROR << "Exception in bulk insertion: " << e.what();
//        return false;
//    }
}

bool MongoDBHandler::insertAllPackets(const vector<CCSDS_Packet> &packets) {
    try {
        auto start = high_resolution_clock::now();

        // Group by collection and build documents
        std::unordered_map<std::string, std::vector<bsoncxx::document::value>> documentsByCollection;

        for (const auto& packet : packets) {
            std::string collection_name = "SID" + std::to_string(packet.sid);

            bsoncxx::builder::basic::document doc{};
            insertHeader(doc, packet);

            Json::StreamWriterBuilder writer;
            std::string jsonStr = Json::writeString(writer, packet.parsedData);
            bsoncxx::document::value subDocument = bsoncxx::from_json(jsonStr);
            doc.append(bsoncxx::builder::basic::kvp("data", subDocument));

            documentsByCollection[collection_name].push_back(doc.extract());
        }

        // Insert into each collection
        for (const auto& [collection_name, documents] : documentsByCollection) {
            mongocxx::collection collection = database_[collection_name];
            collection.insert_many(documents);
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);

        LOG_INFO << "Inserted " << packets.size() << " packets across "
                 << documentsByCollection.size() << " collections in "
                 << duration.count() << "ms";

        return true;

    } catch (const std::exception& e) {
        LOG_ERROR << "Exception in insertAllPackets: " << e.what();
        return false;
    }
}
