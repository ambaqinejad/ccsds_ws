#ifndef CCSDS_PARSER_CCSDS_PACKET_H
#define CCSDS_PARSER_CCSDS_PACKET_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <variant>
#include <memory>
#include <json/value.h>
#include <unordered_map>

#include "SIDs.h"

using namespace std;


class CCSDS_Packet {
public:

    // Constructor
    CCSDS_Packet();

    // Function to deserialize the 128-byte chunk into a Packet structure
    CCSDS_Packet deserialize_packet(vector<uint8_t> &chunk);

    [[nodiscard]] Json::Value toJson() const;

    template<typename T>
    void mapPayloadToMeaningfulData(size_t offset, const std::string& fieldName);

    void mapBitsToMeaningfulData(size_t &byteOffset, size_t &bitOffset,
                                 size_t bitCount, const std::string &fieldName);

    static uint8_t extractBits(const uint8_t* data,
                         size_t &byteOffset,
                         size_t &bitOffset,
                         size_t bitCount);

    template<typename T>
    T readBigEndian(const uint8_t* data);

    template<typename T>
    T swapEndianIfNeeded(const T& val);

    std::string normalize(const std::string& s) {
        if (s == "uint8" || s == "uint8_t")
            return "uint8_t";
        else if (s == "uint16" || s == "uint16_t")
            return "uint16_t";
        else if (s == "uint32" || s == "uint32_t")
            return "uint32_t";
        else if (s == "uint64" || s == "uint64_t")
            return "uint64_t";
        else if (s == "int8" || s == "int8_t")
            return "int8_t";
        else if (s == "int16" || s == "int16_t")
            return "int16_t";
        else if (s == "int32" || s == "int32_t")
            return "int32_t";
        else if (s == "int64" || s == "int64_t")
            return "int64_t";
        else if (s == "float" || s == "float_t")
            return "float_t";
        else if (s == "double" || s == "double_t")
            return "double_t";
        return s;
    }

    // private:
    uint16_t main_frame_header;
    uint16_t packet_id;
    uint16_t packet_sequence_control;
    uint16_t packet_length;
    uint8_t data_field_header;
    uint8_t service_type;
    uint8_t sub_service_type;
    uint8_t sid;
    uint32_t timestamp;
    uint64_t crc_fail_upload_map;
    uint32_t flash_address;
    vector<uint8_t> payload;
    Json::Value parsedData;
};

#endif // CCSDS_PARSER_CCSDS_PACKET_H