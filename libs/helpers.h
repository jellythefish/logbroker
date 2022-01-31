#pragma once

#include <cstdlib>
#include <string>

std::string GetEnvOrDefault(const std::string& key, const std::string& defaultValue="");
