//
// Created by ambaqinejad on 12/17/25.
//

#include "AppConfiguration.h"

#include "Constants.h"

void AppConfiguration::configureCors(drogon::HttpAppFramework &app) {
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

    app.registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req,
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
            .addListener("0.0.0.0", port)
            .setDocumentRoot(documentRoot)
            .run();
}
