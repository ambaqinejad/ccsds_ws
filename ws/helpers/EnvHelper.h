//
// Created by ambaqinejad on 10/29/25.
//

#ifndef FINAL_PROJECT_ENVHELPER_H
#define FINAL_PROJECT_ENVHELPER_H
#include <string>
#include <utility>

class EnvHelper {
public:
    static std::string readEnvVariable(const char* variableName,
                                       const std::string& alternative) {
        const char* environment_variable_value = std::getenv(variableName);
        return environment_variable_value ? environment_variable_value : alternative;
    }
};


#endif //FINAL_PROJECT_ENVHELPER_H
