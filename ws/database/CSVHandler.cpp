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
#include <chrono>
using namespace std::chrono;
namespace fs = std::filesystem;

CSVHandler::CSVHandler() = default;

bool CSVHandler::insertPacket(const CCSDS_Packet &packet, const string& fileUUID) {
    auto start = high_resolution_clock::now();
    string directoryBasePath = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                                          "/home/ambaqinejad/Desktop/drogon_ccsds/ccsds_final_project/ws/public");
    std::string filename = "SID" + std::to_string(packet.sid) + ".csv";
    std::string directoryPath = directoryBasePath + "/" + fileUUID;
    if (!fs::exists(directoryPath)) {
        if (!fs::create_directory(directoryPath)) {
            LOG_INFO << "Failed to create directory: " << directoryPath;
            return false;
        }
    }
    std::string filePath = directoryPath + "/" + filename;
    // Open file in append mode
    std::ofstream csvFile(filePath, std::ios::app);
    if (!csvFile.is_open()) {
        std::cerr << "Failed to open CSV file." << std::endl;
        return false;
    }

    // Collect all keys from parsedData
    std::vector<std::string> keys;
    if (!packet.parsedData.isObject()) {
        std::cerr << "parsedData is not an object!" << std::endl;
        return false;
    }
    keys = packet.parsedData.getMemberNames();
    
    // Check if header already exists
    std::ifstream checkFile(filePath);
    bool isEmpty = (checkFile.peek() == std::ifstream::traits_type::eof());
    checkFile.close();

    if (isEmpty) {
        // Write header
        csvFile << "main_frame_header,packet_id,packet_sequence_control,packet_length,"
                   "data_field_header,service_type,sub_service_type,sid,timestamp,"
                   "crc_fail_upload_map,flash_address";
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
