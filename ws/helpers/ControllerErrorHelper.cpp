//
// Created by ambaqinejad on 4/22/25.
//

#include "ControllerErrorHelper.h"

void ControllerErrorHelper::sendError(std::function<void(const HttpResponsePtr &)> &&callback,
                                      HttpStatusCode statusCode, const string& message) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(statusCode);
    resp->setBody(message);
    callback(resp);
}
