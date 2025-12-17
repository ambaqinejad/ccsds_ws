#include <drogon/drogon.h>
#include <iostream>
#include "database/MongoDBHandler.h"
#include "helpers/AppConfiguration.h"
#include "helpers/StructureHelper.h"

using namespace std;

#include <drogon/HttpFilter.h>
#include "helpers/EnvHelper.h"
#include "helpers/Constants.h"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, const char *argv[]) {
    string documentRoot = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                          Constants::DEFAULT_DOCUMENT_ROOT);
    string uploadPath = documentRoot + Constants::DEFAULT_UPLOAD_DIR;
    MongoDBHandler dbHandler;
    dbHandler.loadStructure();

    if (!fs::exists(documentRoot)) {
        if (!fs::create_directory(documentRoot)) {
            LOG_INFO << Constants::SERVER_COULD_NOT_START_PUBLIC_DIR;
            return -1;
        }
    }

    if (!fs::exists(uploadPath)) {
        if (!fs::create_directory(uploadPath)) {
            LOG_INFO << Constants::SERVER_COULD_NOT_START_UPLOAD_DIR;
            return -1;
        }
    }

    int port = 5000;
    if (argc > 1) port = stoi(argv[1]);
    LOG_INFO << Constants::SERVER_START_ON_PORT << port;
    auto &app = drogon::app();
    AppConfiguration::configureCors(app);
    AppConfiguration::configureServer(app, uploadPath, documentRoot, port);
    return 0;
}
