//
// Created by ambaqinejad on 4/19/25.
//

#include "UIDGeneratorHelper.h"
#include <chrono>

std::string UIDGeneratorHelper::generateUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << std::hex << dis(gen);
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::string uuid = std::to_string(timestamp) + ss.str();
    return uuid;
}
