//
// Created by ambaqinejad on 12/1/25.
//

#ifndef FINAL_PROJECT_CONSTANTS_H
#define FINAL_PROJECT_CONSTANTS_H

#include <string>

class Constants {
public:
    // PATH
    inline static const std::string DEFAULT_DOCUMENT_ROOT = "/home/ambaqinejad/Desktop/drogon_ccsds/ccsds_ws2/ws/public";
    inline static const std::string DEFAULT_UPLOAD_DIR = "/uploads";

    // SERVER ERROR
    inline static const std::string SERVER_COULD_NOT_START_STRUCTURE = "Websocket server could not start because structure did not load.";
    inline static const std::string SERVER_COULD_NOT_START_PUBLIC_DIR = "Websocket server could not start because it can not create public directory.";
    inline static const std::string SERVER_COULD_NOT_START_UPLOAD_DIR = "Websocket server could not start because it can not create upload directory.";
    inline static const std::string STRUCTURE_COULD_NOT_LOAD_FROM_DB_TO_RAM = "Structure could not load from DB to RAM!";
    inline static const std::string FAILED_TO_CREATE_OUTPUT_FILE = "Failed to create output file: ";
    inline static const std::string FAILED_TO_READ_CHUNK = "Failed to read chunk: ";

    // SERVER INFO
    inline static const std::string SERVER_START_ON_PORT = "Starting WebSocket server on port ";
    inline static const std::string STRUCTURE_LOADED_FROM_DB_TO_RAM = "Structure loaded successfully from DB to RAM!";
    inline static const std::string UPLOADED_FILE_HAVE_BEEN_SAVED_TO_THE = "The uploaded file has been saved to the ";

    // RESPONSE HEADER
    inline static const std::string ACCESS_CONTROL_ALLOW_ORIGIN_KEY = "Access-Control-Allow-Origin";
    inline static const std::string ACCESS_CONTROL_ALLOW_METHOD_KEY = "Access-Control-Allow-Methods";
    inline static const std::string ACCESS_CONTROL_ALLOW_HEADER_KEY = "Access-Control-Allow-Headers";
    inline static const std::string ACCESS_CONTROL_ALLOW_ALL_METHOD_VALUE = "GET, POST, PUT, DELETE, OPTIONS";
    inline static const std::string ACCESS_CONTROL_ALLOW_HEADER_VALUE = "Content-Type, Authorization, X-Requested-With, x-signalr-user-agent";
    inline static const std::string ALL = "*";

    // MONGODB
    inline static const std::string MONGODB_DEFAULT_URI = "mongodb://192.168.102.94:27017";
    inline static const std::string DEFAULT_DB_NAME = "Parser";
    inline static const std::string DEFAULT_COLLECTION_NAME = "CCSDS_Structure";
    inline static const std::string DEFAULT_HISTORY_COLLECTION_NAME = "CCSDS_STRUCTURE_HISTORY";
    inline static const std::string DEFAULT_PAYLOAD_COLLECTION_NAME = "PayloadSID";

    // PACKET
    inline static const std::string PACKET_HEADER_KEY_MAIN_FRAME_HEADER = "main_frame_header";
    inline static const std::string PACKET_HEADER_KEY_PACKET_ID = "packet_id";
    inline static const std::string PACKET_HEADER_KEY_PACKET_SEQUENCE_CONTROL = "packet_sequence_control";
    inline static const std::string PACKET_HEADER_KEY_PACKET_LENGTH = "packet_length";
    inline static const std::string PACKET_HEADER_KEY_DATA_FIELD_HEADER = "data_field_header";
    inline static const std::string PACKET_HEADER_KEY_SERVICE_TYPE = "service_type";
    inline static const std::string PACKET_HEADER_KEY_SUB_SERVICE_TYPE = "sub_service_type";
    inline static const std::string PACKET_HEADER_KEY_SID = "Sid";
    inline static const std::string PACKET_HEADER_KEY_TIMESTAMP = "timestamp";
    inline static const std::string PACKET_HEADER_KEY_CRC_FAIL_UPLOAD_MAP = "crc_fail_upload_map";
    inline static const std::string PACKET_HEADER_KEY_FLASH_ADDRESS = "flash_address";
    inline static const std::string PACKET_HEADER_KEY_TRACK_ID = "TrackID";
    inline static const std::string PACKET_HEADER_KEY_STATION_DATE_TIME = "StationDateTime";

    // API
    inline static const std::string FILE_UPLOAD = "/upload";
    inline static const std::string FILE_START_UPLOAD = "/startUpload";
    inline static const std::string FILE_UPLOAD_CHUNK = "/uploadChunk";
    inline static const std::string FILE_FINALIZE_UPLOAD = "/finalizeUpload";

    // API_ERROR
    inline static const std::string MUST_ONLY_BE_ONE_FILE = "Must only be one file";
    inline static const std::string ERROR_IN_STARTING_UPLOAD = "Error in starting upload.";
    inline static const std::string INVALID_UPLOAD_DATA = "Invalid upload data";
    inline static const std::string NO_FILE_CHUNK_FOUND = "No file chunk found";
    inline static const std::string UPLOAD_SESSION_NOT_FOUND = "Upload session not found";
    inline static const std::string STRUCTURE_NOT_FOUND = "Structure not found";

    // API_RESPONSE
    inline static const std::string UPLOADED_FILE_HAVE_BEEN_SAVED = "The uploaded file has been saved";
    inline static const std::string FILE_UPLOAD_COMPLETED_AND_PROCESSING_STARTED = "File upload completed and processing started";

};
#endif //FINAL_PROJECT_CONSTANTS_H















