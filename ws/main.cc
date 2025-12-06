#include <drogon/drogon.h>
#include <iostream>
#include "database/MongoDBHandler.h"
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
    if (!dbHandler.loadStructure()) {
        LOG_INFO << Constants::SERVER_COULD_NOT_START_STRUCTURE;
        return 0;
    }

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

    // 1️⃣ Handle OPTIONS (CORS preflight)
    app.registerPreRoutingAdvice([](const drogon::HttpRequestPtr &req,
                                    drogon::AdviceCallback &&acb,
                                    drogon::AdviceChainCallback &&accb) {
        if (req->getMethod() == drogon::Options) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_ORIGIN_KEY, Constants::ALL);
            resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_METHOD_KEY, Constants::ACCESS_CONTROL_ALLOW_ALL_METHOD_VALUE);
            resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_HEADER_KEY, Constants::ACCESS_CONTROL_ALLOW_HEADER_VALUE);
            resp->setStatusCode(drogon::k200OK);
            acb(resp);  // Send immediate response for OPTIONS
        } else {
            accb();  // Continue normal routing
        }
    });

    // 3️⃣ Add CORS headers to all valid responses
    app.registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req,
                                      const drogon::HttpResponsePtr &resp) {
        resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_ORIGIN_KEY, Constants::ALL);
        resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_METHOD_KEY, Constants::ACCESS_CONTROL_ALLOW_ALL_METHOD_VALUE);
        resp->addHeader(Constants::ACCESS_CONTROL_ALLOW_HEADER_KEY, Constants::ACCESS_CONTROL_ALLOW_HEADER_VALUE);
    });

    // 4️⃣ Configure Drogon app
    app.setClientMaxBodySize(200 * 2000 * 2000)
            .setUploadPath(uploadPath)
            .addListener("0.0.0.0", port)
            .setDocumentRoot(documentRoot)
            .run();
    return 0;
}
