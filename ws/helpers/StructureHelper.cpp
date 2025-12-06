#include <fstream>
#include <nlohmann/json.hpp>  // JSON for Modern C++
#include "StructureHelper.h"
#include "database/MongoDBHandler.h"
#include <drogon/HttpController.h>


// Function to convert JSON to C++ structs
std::string StructureHelper::generateStructsFromJsonAndStoreInDB(const std::string& jsonFilePath) {
    // Read JSON file
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        LOG_INFO << "Failed to open JSON file!\n";
        return "Failed to open JSON file!";
    }

    nlohmann::ordered_json j;
    jsonFile >> j;

    MongoDBHandler dbHandler;
    dbHandler.insertStructure(j);

    LOG_INFO << "Structs generated successfully into db.\n";
    return "Structs generated successfully into db.";
}