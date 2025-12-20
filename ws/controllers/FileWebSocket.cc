#include "FileWebSocket.h"
#include "FileController.h"
#include "helpers/ClientCommunicationHelper.h"
#include "helpers/EnvHelper.h"
#include "helpers/WorkingWithFileSystem.h"

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
    string uploadPath = EnvHelper::readEnvVariable("UPLOAD_DIR",
                          Constants::DEFAULT_UPLOAD_DIR);
    string csvPath = EnvHelper::readEnvVariable("CSV_DIR",
                                          Constants::DEFAULT_CSV_DIR);
    WorkingWithFileSystem::deleteFile(uploadPath, "");
    WorkingWithFileSystem::deleteCSVs(csvPath, "");
    ClientCommunicationHelper::shouldStopProcessing = true;
}
