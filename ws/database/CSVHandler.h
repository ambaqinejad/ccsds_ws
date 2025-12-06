//
// Created by ambaqinejad on 4/16/25.
//

#ifndef FINAL_PROJECT_CSVHANDLER_H
#define FINAL_PROJECT_CSVHANDLER_H
#include "logics/CCSDS_Packet.h"


class CSVHandler {
public:
    CSVHandler();

    static bool insertPacket(const CCSDS_Packet &packet, const string& fileUUID);


private:
    std::string csvFilePath;
};

#endif //FINAL_PROJECT_CSVHANDLER_H
