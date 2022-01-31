#include "helpers.h"

std::string GetEnvOrDefault(const std::string& key, const std::string& defaultValue) {
    auto value = std::getenv(key.c_str());
    return value ? value : defaultValue;
}
