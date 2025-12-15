#include "FileWebSocket.h"
#include "FileController.h"
#include "helpers/ClientCommunicationHelper.h"

void FileWebSocket::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type) {

}

void FileWebSocket::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr& wsConnPtr)
{
    // write your application logic here
    LOG_INFO << "New Web Socket Connection";
    ClientCommunicationHelper::clients.insert(wsConnPtr);
    ClientCommunicationHelper::shouldStopProcessing = false;
}

void FileWebSocket::handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr) {
    LOG_INFO << "Socket closed";
    ClientCommunicationHelper::clients.erase(wsConnPtr);
    ClientCommunicationHelper::shouldStopProcessing = true;
}
