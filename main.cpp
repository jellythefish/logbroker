#include <drogon/drogon.h>

#include <cstdlib>
#include <ostream>

std::string GetEnvOrDefault(const std::string& key, const std::string& defaultValue="") {
    auto value = std::getenv(key.c_str());
    return value ? value : defaultValue;
}

struct ClickHouseConfig {
    std::string HOST;
    std::string USER;
    std::string PASSWORD;
    int PORT;
    std::string CERT_PATH;
};

trantor::LogStream& operator <<(trantor::LogStream& out, const ClickHouseConfig& chConfig) {
    return out << "HOST: " << chConfig.HOST << '\n'
                << "USER: " << chConfig.USER << '\n'
                << "PASSWORD: " << chConfig.PASSWORD << '\n'
                << "PORT: " << chConfig.PORT << '\n'
                << "CERT_PATH: " << chConfig.CERT_PATH;
}

ClickHouseConfig GetClickHouseConfig() {
    return {
        .HOST = GetEnvOrDefault("LOGBROKER_CH_HOST", "localhost"),
        .USER = GetEnvOrDefault("LOGBROKER_CH_HOST"),
        .PASSWORD = GetEnvOrDefault("LOGBROKER_CH_PASSWORD"),
        .PORT = std::stoi(GetEnvOrDefault("LOGBROKER_CH_PORT", "8123")),
        .CERT_PATH = GetEnvOrDefault("LOGBROKER_CH_CERT_PATH")
    };
}

int main() {
    ClickHouseConfig chConfig = GetClickHouseConfig();
    LOG_DEBUG << "ClickHouse config is: \n" << chConfig;
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 80);

    //Load config file
    drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();

    return 0;
}
