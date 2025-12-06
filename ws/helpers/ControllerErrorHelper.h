//
// Created by ambaqinejad on 4/22/25.
//

#ifndef FINAL_PROJECT_CONTROLLERERRORHELPER_H
#define FINAL_PROJECT_CONTROLLERERRORHELPER_H

#include <drogon/HttpController.h>

using namespace std;
using namespace drogon;
class ControllerErrorHelper {
public:
    static void sendError(std::function<void(const HttpResponsePtr &)> &&callback,
                          HttpStatusCode statusCode, const string& message);
};


#endif //FINAL_PROJECT_CONTROLLERERRORHELPER_H
