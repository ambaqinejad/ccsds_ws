//
// Created by ambaqinejad on 12/17/25.
//

#ifndef APPCONFIGURATION_H
#define APPCONFIGURATION_H
#include <drogon/HttpAppFramework.h>


class AppConfiguration {
public:
    static bool bootstrap(int port);
private:
    static void configureCors(drogon::HttpAppFramework& app);
    static void configureServer(drogon::HttpAppFramework& app,
        const std::string& uploadPath, const std::string &documentRoot, int port);
    static bool fileAndDirBootstrap(const std::string& documentRoot,
        const std::string& uploadPath,
        const std::string& csvPath);
};



#endif //APPCONFIGURATION_H
