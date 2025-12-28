//
// Created by ambaqinejad on 4/19/25.
//

#include <json/value.h>
#include "ClientCommunicationHelper.h"

std::unordered_set<WebSocketConnectionPtr> ClientCommunicationHelper::clients;

void ClientCommunicationHelper::notifyClients(const double progress) {
    Json::Value msg;
    if (progress == -1) {
        msg["progress"] = "No Progress an Error Occur.";
    } else if (progress == 100) {
        msg["progress"] = progress;
        msg["status"] = "Completed";
    } else {
        msg["progress"] = progress;
    }

    for (const auto &client : clients) {
        if (client && client->connected()) {
            client->send(msg.toStyledString());
        }
    }
}