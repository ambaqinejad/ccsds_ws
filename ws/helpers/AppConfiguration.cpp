//
// Created by ambaqinejad on 12/17/25.
//

#include <filesystem>
#include "AppConfiguration.h"
#include "Constants.h"
#include "EnvHelper.h"
#include "database/MongoDBHandler.h"

namespace fs = std::filesystem;

bool AppConfiguration::bootstrap(const int port) {
    const std::string documentRoot = EnvHelper::readEnvVariable("DOCUMENT_ROOT",
                                                                Constants::DEFAULT_DOCUMENT_ROOT);
    const std::string uploadPath = EnvHelper::readEnvVariable("UPLOAD_DIR",
                          Constants::DEFAULT_UPLOAD_DIR);
    const std::string csvPath = EnvHelper::readEnvVariable("CSV_DIR",
                          Constants::DEFAULT_CSV_DIR);
    MongoDBHandler dbHandler;
    dbHandler.loadStructure();
    auto &app = drogon::app();
    if (!fileAndDirBootstrap(documentRoot, uploadPath, csvPath)) {
        return false;
    }
    configureCors(app);
    configureServer(app, uploadPath, documentRoot, port);
    LOG_INFO << Constants::SERVER_START_ON_PORT << port;
    return true;
}

void AppConfiguration::configureCors(drogon::HttpAppFramework &app) {
    app.registerPreRoutingAdvice([](const drogon::HttpRequestPtr &req,
                                    drogon::AdviceCallback &&acb,
                                    drogon::AdviceChainCallback &&accb) {
        if (req->getMethod() == drogon::Options) {
            const auto resp = drogon::HttpResponse::newHttpResponse();
            resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_ORIGIN_KEY, Constants::ALL);
            resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_METHOD_KEY, Constants::ACCESS_CONTROL_ALLOW_ALL_METHOD_VALUE);
            resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_HEADER_KEY, Constants::ACCESS_CONTROL_ALLOW_HEADER_VALUE);
            resp->setStatusCode(drogon::k200OK);
            acb(resp);  // Send immediate response for OPTIONS
        } else {
            accb();  // Continue normal routing
        }
    });

    app.registerPostHandlingAdvice([](const drogon::HttpRequestPtr &,
                                      const drogon::HttpResponsePtr &resp) {
        resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_ORIGIN_KEY, Constants::ALL);
        resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_METHOD_KEY, Constants::ACCESS_CONTROL_ALLOW_ALL_METHOD_VALUE);
        resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_HEADER_KEY, Constants::ACCESS_CONTROL_ALLOW_HEADER_VALUE);
    });
}

void AppConfiguration::configureServer(drogon::HttpAppFramework &app,
    const std::string& uploadPath, const std::string &documentRoot, const int port) {
    app.setClientMaxBodySize(200 * 2000 * 2000)
            .setUploadPath(uploadPath)
            .addListener(Constants::LISTENING_IP, port)
            .setDocumentRoot(documentRoot)
            .run();
}

bool AppConfiguration::fileAndDirBootstrap(const std::string &documentRoot, const std::string &uploadPath,
    const std::string &csvPath) {
    if (!fs::exists(documentRoot)) {
        if (!fs::create_directory(documentRoot)) {
            LOG_INFO << Constants::SERVER_COULD_NOT_START_PUBLIC_DIR;
            return false;
        }
    }
    if (!fs::exists(uploadPath)) {
        if (!fs::create_directory(uploadPath)) {
            LOG_INFO << Constants::SERVER_COULD_NOT_START_UPLOAD_DIR;
            return false;
        }
    }
    if (!fs::exists(csvPath)) {
        if (!fs::create_directory(csvPath)) {
            LOG_INFO << Constants::SERVER_COULD_NOT_START_UPLOAD_DIR;
            return false;
        }
    }
    return true;
}
