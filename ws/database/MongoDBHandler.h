//
// Created by ambaqinejad on 3/27/25.
//

#ifndef MONGODBHANDLER_H
#define MONGODBHANDLER_H

#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <nlohmann/json.hpp>
#include "logics/CCSDS_Packet.h"


class MongoDBHandler {
public:
    MongoDBHandler();

    bool insertPacket(const CCSDS_Packet &packet);
    bool insertAllPackets(const std::vector<CCSDS_Packet>& packets);
    bool insertPacketsBulk(const std::vector<CCSDS_Packet>& packets, size_t batchSize = 1000);
    void insertStructure(nlohmann::ordered_json json);
    bool loadStructure();
    static nlohmann::ordered_json ccsds_structure_;

private:
    mongocxx::client client_;
    mongocxx::database database_;

    static void insertHeader(bsoncxx::builder::basic::document& document, const CCSDS_Packet &packet);
};


#endif //MONGODBHANDLER_H
