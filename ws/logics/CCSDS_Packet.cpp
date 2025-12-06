#include "CCSDS_Packet.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <arpa/inet.h>
#include "database/MongoDBHandler.h"

CCSDS_Packet::CCSDS_Packet() {
    main_frame_header = 0;
    packet_id = 0;
    packet_sequence_control = 0;
    packet_length = 0;
    data_field_header = 0;
    service_type = 0;
    sub_service_type = 0;
    sid = 0;
    timestamp = 0;
    crc_fail_upload_map = 0;
    flash_address = 0;
}

CCSDS_Packet CCSDS_Packet::deserialize_packet(vector<uint8_t> &chunk) {
    try {

        CCSDS_Packet packet;
        size_t offset = 0;
        size_t bitOffset = 0;

        auto read_uint16 = [&](size_t &offset) {
            uint16_t value;
            value = readBigEndian<uint16_t>(chunk.data() + offset);
            offset += sizeof(value);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << value << std::endl;
            std::cout << std::hex << std::setw(4) << std::setfill('0') << ntohs(value) << std::endl;
            return value;
        };

        auto read_uint32 = [&](size_t &offset) {
            uint32_t value;
            value = readBigEndian<uint32_t>(chunk.data() + offset);
            offset += sizeof(value);
            // return ntohs(value);
            std::cout << std::hex << std::setw(8) << std::setfill('0') << value << std::endl;
            std::cout << std::hex << std::setw(8) << std::setfill('0') << ntohl(value) << std::endl;

            return value;
        };

        auto read_uint64 = [&](size_t &offset) {
            uint64_t value;
            value = readBigEndian<uint64_t>(chunk.data() + offset);
            offset += sizeof(value);
            // return ntohs(value);
            std::cout << std::hex << std::setw(16) << std::setfill('0') << value << std::endl;
            std::cout << std::hex << std::setw(16) << std::setfill('0') << __builtin_bswap64(value) << std::endl;

            return value;
        };

        static const std::unordered_map<std::string, std::function<void(size_t &, const std::string &)>> handlers = {
                {"uint",     [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<uint>(offset, name);
                    offset += sizeof(uint);
                }},
                {"int",      [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<int>(offset, name);
                    offset += sizeof(int);
                }},
                {"uint8_t",  [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<uint8_t>(offset, name);
                    offset += sizeof(uint8_t);
                }},
                {"int8_t",   [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<int8_t>(offset, name);
                    offset += sizeof(int8_t);
                }},
                {"uint16_t", [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<uint16_t>(offset, name);
                    offset += sizeof(uint16_t);
                }},
                {"int16_t",  [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<int16_t>(offset, name);
                    offset += sizeof(int16_t);
                }},
                {"uint32_t", [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<uint32_t>(offset, name);
                    offset += sizeof(uint32_t);
                }},
                {"int32_t",  [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<int32_t>(offset, name);
                    offset += sizeof(int32_t);
                }},
                {"uint64_t", [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<uint64_t>(offset, name);
                    offset += sizeof(uint64_t);
                }},
                {"int64_t",  [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<int64_t>(offset, name);
                    offset += sizeof(int64_t);
                }},
                {"double_t", [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<double_t>(offset, name);
                    offset += sizeof(double_t);
                }},
                {"float_t",  [&](size_t &offset, const std::string &name) {
                    mapPayloadToMeaningfulData<float_t>(offset, name);
                    offset += sizeof(float_t);
                }},
        };

        // Read fixed fields
        packet.main_frame_header = read_uint16(offset);
        packet.packet_id = read_uint16(offset);
        packet.packet_sequence_control = read_uint16(offset);
        packet.packet_length = read_uint16(offset);
        packet.data_field_header = chunk[offset++];
        packet.service_type = chunk[offset++];
        packet.sub_service_type = chunk[offset++];
        packet.sid = chunk[offset++];
        packet.timestamp = read_uint32(offset);
        packet.crc_fail_upload_map = read_uint64(offset);
        packet.flash_address = read_uint32(offset);

        // Remaining bytes are payload
        packet.payload.assign(chunk.begin() + offset, chunk.end());
        this->payload.assign(chunk.begin() + offset, chunk.end());
        offset = 0;
        auto it = std::find_if(
                MongoDBHandler::ccsds_structure_.begin(),
                MongoDBHandler::ccsds_structure_.end(),
                [&](const nlohmann::ordered_json &obj) {
                    std::string sid_str = obj["metadata"]["SID"].get<std::string>();
                    if (sid_str.rfind("SID", 0) == 0) {   // starts with "SID"
                        sid_str = sid_str.substr(3);     // keep only the number part
                    }
                    int sid_val = std::stoi(sid_str);
                    return sid_val == packet.sid;
                }
        );

        if (it == MongoDBHandler::ccsds_structure_.end()) {
            return packet;
        }
        for (auto topple = it->begin(); topple != it->end(); ++topple) {
            const std::string &fieldName = topple.key();
            if (fieldName == "_id" || fieldName == "metadata") {
                continue;
            }
            nlohmann::ordered_json fieldData = topple.value();
            std::string fieldType = fieldData["type"];
            auto handler = handlers.find(normalize(fieldType));
            if (handler != handlers.end()) {
                bitOffset = 0;
                handler->second(offset, fieldName);
            }
            if (fieldType.rfind("bit", 0) == 0) {
                size_t bitCount = std::stoi(fieldType.substr(3));
                if (bitCount < 1 || bitCount > 7)
                    throw std::runtime_error("bit count must be 1-7");
                mapBitsToMeaningfulData(offset, bitOffset, bitCount, fieldName);
            }
        }

        packet.parsedData = parsedData;
        return packet;
    } catch (const std::exception &e) {
        cout << e.what();
    }
}

Json::Value CCSDS_Packet::toJson() const {
    Json::Value msg;
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');

    auto formatHex = [&](int value, int width = 2) {
        ss.str("");
        ss.clear();
        ss << std::setw(width) << (value & 0xFF);
        return ss.str();
    };

    msg["main_frame_header"] = formatHex((main_frame_header >> 8) & 0xFF) + " " + formatHex(main_frame_header & 0xFF);
    msg["packet_id"] = formatHex((packet_id >> 8) & 0xFF) + " " + formatHex(packet_id & 0xFF);
    msg["packet_sequence_control"] =
            formatHex((packet_sequence_control >> 8) & 0xFF) + " " + formatHex(packet_sequence_control & 0xFF);
    msg["packet_length"] = formatHex((packet_length >> 8) & 0xFF) + " " + formatHex(packet_length & 0xFF);
    msg["data_field_header"] = formatHex(data_field_header);
    msg["service_type"] = formatHex(service_type);
    msg["sub_service_type"] = formatHex(sub_service_type);
    msg["sid"] = formatHex(sub_service_type);

    msg["timestamp"] = formatHex((timestamp >> 24) & 0xFF) + " " +
                       formatHex((timestamp >> 16) & 0xFF) + " " +
                       formatHex((timestamp >> 8) & 0xFF) + " " +
                       formatHex(timestamp & 0xFF);

    msg["crc_fail_upload_map"] = formatHex((crc_fail_upload_map >> 56) & 0xFF) + " " +
                                 formatHex((crc_fail_upload_map >> 48) & 0xFF) + " " +
                                 formatHex((crc_fail_upload_map >> 40) & 0xFF) + " " +
                                 formatHex((crc_fail_upload_map >> 32) & 0xFF) + " " +
                                 formatHex((crc_fail_upload_map >> 24) & 0xFF) + " " +
                                 formatHex((crc_fail_upload_map >> 16) & 0xFF) + " " +
                                 formatHex((crc_fail_upload_map >> 8) & 0xFF) + " " +
                                 formatHex(crc_fail_upload_map & 0xFF);

    msg["flash_address"] = formatHex((flash_address >> 24) & 0xFF) + " " +
                           formatHex((flash_address >> 16) & 0xFF) + " " +
                           formatHex((flash_address >> 8) & 0xFF) + " " +
                           formatHex(flash_address & 0xFF);

    // Optional: Dump payload as hex or array
    Json::Value parsedDataObj(Json::objectValue);
    parsedDataObj = parsedData;
    msg["payload"] = parsedDataObj;

    return msg;
}

void
CCSDS_Packet::mapBitsToMeaningfulData(size_t &byteOffset, size_t &bitOffset, size_t bitCount, const string &fieldName) {
    uint8_t bits = extractBits(payload.data(), byteOffset, bitOffset, bitCount);
    parsedData[fieldName] = bits;
}

uint8_t CCSDS_Packet::extractBits(const uint8_t *data, size_t &byteOffset, size_t &bitOffset, size_t bitCount) {
    uint8_t value = 0;
    size_t bitsRead = 0;

    while (bitsRead < bitCount) {
        size_t bitsLeftInByte = 8 - bitOffset;
        size_t bitsToRead = std::min(bitCount - bitsRead, bitsLeftInByte);

        uint8_t currentByte = data[byteOffset];

        uint8_t mask = (1u << bitsToRead) - 1;
        uint8_t extracted =
                (currentByte >> (bitsLeftInByte - bitsToRead)) & mask;

        value = static_cast<uint8_t>((value << bitsToRead) | extracted);

        bitOffset += bitsToRead;
        bitsRead  += bitsToRead;

        if (bitOffset == 8) {
            bitOffset = 0;
            byteOffset++;
        }
    }

    return value;   // value is 0..127 for bitCount ≤ 7
}

//template<typename T>
//void CCSDS_Packet::mapPayloadToMeaningfulData(size_t offset, const string &fieldName) {
//    T value;
//    std::memcpy(&value, payload.data() + offset, sizeof(T));
//    if (isnan(value)) {
//        value = 0;
//    }
////    std::cout << std::hex << std::setw(4) << std::setfill('0') << value << std::endl;
////    std::cout << std::hex << std::setw(4) << std::setfill('0') << ntohs(value) << std::endl;
//
//    parsedData[fieldName] = value; // Implicitly constructs a FieldValue
//}

template<typename T>
void CCSDS_Packet::mapPayloadToMeaningfulData(size_t offset, const std::string &fieldName) {
    T value = readBigEndian<T>(payload.data() + offset);
    parsedData[fieldName] = value; // Construct FieldValue implicitly
}

template<typename T>
T CCSDS_Packet::swapEndianIfNeeded(const T& val) {
    T result = val;

    if constexpr (std::is_integral_v<T>) {
        if constexpr (sizeof(T) == 2) {         // 16-bit
            result = ntohs(val);
        } else if constexpr (sizeof(T) == 4) {  // 32-bit
            result = ntohl(val);
        } else if constexpr (sizeof(T) == 8) {  // 64-bit
#if defined(__APPLE__) || defined(__MACH__)
            result = OSSwapBigToHostInt64(val); // on macOS
#else
            // Generic manual 64-bit swap
            uint64_t tmp;
            std::memcpy(&tmp, &val, sizeof(tmp));
            tmp = ((tmp & 0x00000000000000FFULL) << 56) |
                  ((tmp & 0x000000000000FF00ULL) << 40) |
                  ((tmp & 0x0000000000FF0000ULL) << 24) |
                  ((tmp & 0x00000000FF000000ULL) << 8) |
                  ((tmp & 0x000000FF00000000ULL) >> 8) |
                  ((tmp & 0x0000FF0000000000ULL) >> 24) |
                  ((tmp & 0x00FF000000000000ULL) >> 40) |
                  ((tmp & 0xFF00000000000000ULL) >> 56);
            std::memcpy(&result, &tmp, sizeof(result));
#endif
        }
        // 1-byte types (uint8_t, int8_t) do not need swapping
    }
    // floats/doubles will be handled separately
    return result;
}

template<typename T>
T CCSDS_Packet::readBigEndian(const uint8_t* data) {
    T value;
    std::memcpy(&value, data, sizeof(T));

    if constexpr (std::is_floating_point_v<T>) {
        // Reverse bytes for big-endian payload
        uint8_t tmp[sizeof(T)];
        std::reverse_copy(reinterpret_cast<uint8_t*>(&value),
                          reinterpret_cast<uint8_t*>(&value) + sizeof(T),
                          tmp);
        std::memcpy(&value, tmp, sizeof(T));
    } else {
        // Integer types
        value = swapEndianIfNeeded(value);
    }

    // Handle NaN for floats/doubles
    if constexpr (std::is_floating_point_v<T>) {
        if (std::isnan(value)) value = 0;
    }

    return value;
}

//template<typename T>
//T CCSDS_Packet::readBigEndian(const uint8_t* data) {
//    T value;
//    std::memcpy(&value, data, sizeof(T));
//
//    // integers may need swapping (if CCSDS defines them as big-endian)
//    if constexpr (std::is_integral_v<T> && sizeof(T) > 1) {
//        uint8_t* p = reinterpret_cast<uint8_t*>(&value);
//        std::reverse(p, p + sizeof(T));  // ONLY if integer is big-endian
//    }
//
//    // floats/doubles stay AS-IS
//    return value;
//}




