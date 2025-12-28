////
//// Created by ambaqinejad on 4/16/25.
////
//
#include "CSVHandler.h"
#include <fstream>
#include "logics/CCSDS_Packet.h"
#include <drogon/drogon.h>
#include <filesystem>
#include "helpers/EnvHelper.h"
#include "helpers/Constants.h"
#include <chrono>
using namespace std::chrono;
namespace fs = std::filesystem;

CSVHandler::CSVHandler() = default;

bool CSVHandler::insertPacket(const CCSDS_Packet &packet, const string& fileUUID) {
    auto start = high_resolution_clock::now();
    string csvPath = EnvHelper::readEnvVariable("CSV_DIR",
                                          Constants::DEFAULT_CSV_DIR);
    std::string filename = "SID" + std::to_string(packet.sid) + ".csv";
    std::string directoryPath = csvPath + "/" + fileUUID;
    if (!fs::exists(directoryPath)) {
        if (!fs::create_directory(directoryPath)) {
            LOG_ERROR << Constants::FAILED_TO_CREATE_DIRECTORY << directoryPath;
            return false;
        }
    }
    std::string filePath = directoryPath + "/" + filename;
    LOG_INFO << "CSV File Path: " << filePath;
    // Open file in append mode
    std::ofstream csvFile(filePath, std::ios::app);
    if (!csvFile.is_open()) {
        LOG_ERROR << Constants::FAILED_TO_OPEN_CSV_FILE;
        return false;
    }

    // Collect all keys from parsedData
    std::vector<std::string> keys;
    if (!packet.parsedData.isObject()) {
        LOG_ERROR << Constants::PARSED_DATA_IS_NOT_AN_OBJECT;
        return false;
    }
    keys = packet.parsedData.getMemberNames();
    
    // Check if header already exists
    std::ifstream checkFile(filePath);
    bool isEmpty = (checkFile.peek() == std::ifstream::traits_type::eof());
    checkFile.close();

    if (isEmpty) {
        // Write header
        csvFile <<    Constants::PACKET_HEADER_KEY_MAIN_FRAME_HEADER + ","
                    + Constants::PACKET_HEADER_KEY_PACKET_ID + ","
                    + Constants::PACKET_HEADER_KEY_PACKET_SEQUENCE_CONTROL + ","
                    + Constants::PACKET_HEADER_KEY_PACKET_LENGTH + ","
                    + Constants::PACKET_HEADER_KEY_DATA_FIELD_HEADER + ","
                    + Constants::PACKET_HEADER_KEY_SERVICE_TYPE + ","
                    + Constants::PACKET_HEADER_KEY_SUB_SERVICE_TYPE + ","
                    + Constants::PACKET_HEADER_KEY_SID + ","
                    + Constants::PACKET_HEADER_KEY_TIMESTAMP + ","
                    + Constants::PACKET_HEADER_KEY_CRC_FAIL_UPLOAD_MAP + ","
                    + Constants::PACKET_HEADER_KEY_FLASH_ADDRESS;

        for (const auto& key : keys) {
            csvFile << ',' << key;
        }
        csvFile << '\n';
    }

    // Write values
    csvFile << static_cast<int>(packet.main_frame_header) << ',' << static_cast<int>(packet.packet_id) << ",";
    csvFile << static_cast<int>(packet.packet_sequence_control) << ',' << static_cast<int>(packet.packet_length) << ",";
    csvFile << static_cast<int>(packet.data_field_header) << ',' << static_cast<int>(packet.service_type) << ",";
    csvFile << static_cast<int>(packet.sub_service_type) << ',' << static_cast<int>(packet.sid) << ",";
    csvFile << static_cast<int>(packet.timestamp) << ',' << static_cast<int>(packet.crc_fail_upload_map) << ",";
    csvFile << static_cast<int>(packet.flash_address);
    for (const auto& key : keys) {
        csvFile << ',' << packet.parsedData[key].asString();
    }
    csvFile << '\n';
    csvFile.close();
    auto end = high_resolution_clock::now();
    auto duration_us = duration_cast<microseconds>(end - start);  // microseconds
    LOG_INFO << "Packet inserted successfully. Time (microseconds): " << duration_us.count() << " µs";
    return true;
}
