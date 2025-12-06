//
// Created by ambaqinejad on 4/19/25.
//

#include <json/value.h>
#include "ClientCommunicationHelper.h"

std::unordered_set<WebSocketConnectionPtr> ClientCommunicationHelper::clients;

void ClientCommunicationHelper::notifyClients(double progress) {
    Json::Value msg;
    if (progress == -1) {
        msg["progress"] = "No Progress an Error Occur.";
    } else if (progress == 100) {
        msg["progress"] = progress;
        msg["status"] = "Completed";
    } else {
        msg["progress"] = progress;
    }

//    std::stringstream ss;
//    ss << std::hex << std::uppercase << std::setfill('0');
//
//    auto formatHex = [&](int value, int width = 2) {
//        ss.str("");
//        ss.clear();
//        ss << std::setw(width) << (value & 0xFF);
//        return ss.str();
//    };
//
//    msg["main_frame_header"] = formatHex((packet.main_frame_header >> 8) & 0xFF) + " " + formatHex(packet.main_frame_header & 0xFF);
//    msg["packet_id"] = formatHex((packet.packet_id >> 8) & 0xFF) + " " + formatHex(packet.packet_id & 0xFF);
//    msg["packet_sequence_control"] = formatHex((packet.packet_sequence_control >> 8) & 0xFF) + " " + formatHex(packet.packet_sequence_control & 0xFF);
//    msg["packet_length"] = formatHex((packet.packet_length >> 8) & 0xFF) + " " + formatHex(packet.packet_length & 0xFF);
//    msg["data_field_header"] = formatHex(packet.data_field_header);
//    msg["service_type"] = formatHex(packet.service_type);
//    msg["sub_service_type"] = formatHex(packet.sub_service_type);
//    msg["sid"] = formatHex(packet.sub_service_type);
//
//    msg["timestamp"] = formatHex((packet.timestamp >> 24) & 0xFF) + " " +
//                       formatHex((packet.timestamp >> 16) & 0xFF) + " " +
//                       formatHex((packet.timestamp >> 8) & 0xFF) + " " +
//                       formatHex(packet.timestamp & 0xFF);
//
//    msg["crc_fail_upload_map"] = formatHex((packet.crc_fail_upload_map >> 56) & 0xFF) + " " +
//                                 formatHex((packet.crc_fail_upload_map >> 48) & 0xFF) + " " +
//                                 formatHex((packet.crc_fail_upload_map >> 40) & 0xFF) + " " +
//                                 formatHex((packet.crc_fail_upload_map >> 32) & 0xFF) + " " +
//                                 formatHex((packet.crc_fail_upload_map >> 24) & 0xFF) + " " +
//                                 formatHex((packet.crc_fail_upload_map >> 16) & 0xFF) + " " +
//                                 formatHex((packet.crc_fail_upload_map >> 8) & 0xFF) + " " +
//                                 formatHex(packet.crc_fail_upload_map & 0xFF);
//
//    msg["flash_address"] = formatHex((packet.flash_address >> 24) & 0xFF) + " " +
//                           formatHex((packet.flash_address >> 16) & 0xFF) + " " +
//                           formatHex((packet.flash_address >> 8) & 0xFF) + " " +
//                           formatHex(packet.flash_address & 0xFF);

    for (const auto &client : clients) {
        if (client && client->connected()) {
            client->send(msg.toStyledString());
        }
    }
}