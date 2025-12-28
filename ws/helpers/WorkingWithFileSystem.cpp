//
// Created by ambaqinejad on 12/17/25.
//

#include "WorkingWithFileSystem.h"
#include <filesystem>
namespace fs = std::filesystem;

bool WorkingWithFileSystem::deleteFile(const std::string &dirPath, const std::string &fileName) {
    if (fileName.empty() && fs::exists(dirPath)) {
        for (const auto &entry: fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                fs::remove(entry.path());
            }
        }
        return true;
    }
    fs::path dir = dirPath;
    const fs::path file = fileName;

    if (const std::string filePath = (fs::path(dir) / file).string(); fs::exists(filePath) && fs::is_regular_file(filePath)) {
        fs::remove(filePath);
        return true;
    }
    return false;
}

bool WorkingWithFileSystem::deleteCSVs(const std::string &dirPath, const std::string &fileName) {
    if (fileName.empty() && fs::exists(dirPath)) {
        for (const auto& entry : fs::directory_iterator(dirPath))
        {
            fs::remove_all(entry.path());
        }
        return true;
    }
    fs::path dir = dirPath;
    const fs::path file = fileName;

    if (const std::string filePath = (fs::path(dir) / file).string(); fs::exists(filePath) && fs::is_regular_file(filePath)) {
        fs::remove(filePath);
        return true;
    }
    return false;
}
