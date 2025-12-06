//
// Created by ambaqinejad on 4/19/25.
//

#ifndef FINAL_PROJECT_CLIENTCOMMUNICATIONHELPER_H
#define FINAL_PROJECT_CLIENTCOMMUNICATIONHELPER_H


#include "logics/CCSDS_Packet.h"
#include <drogon/WebSocketConnection.h>
#include <drogon/drogon.h>

using namespace drogon;
using namespace std;

class ClientCommunicationHelper {
public:
    static void notifyClients(double progress);
    static unordered_set<WebSocketConnectionPtr> clients;
    static const int progressDivider = 50;
};


#endif //FINAL_PROJECT_CLIENTCOMMUNICATIONHELPER_H
