#include "clickhouse_connector.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

using namespace Logbroker;

ClickHouseConnector::ClickHouseConnector(ClickHouseConnectorConfig chConfig) 
    : CHConfig_(chConfig)
{
}

bool ClickHouseConnector::ClickHouseIsOK() const {
    try {
        LOG_DEBUG << "Making check request to ClickHouse server...";
        curlpp::Cleanup myCleanup;
        curlpp::Easy request;
        std::stringstream response;
        std::stringstream endpoint;

        endpoint << "http://" << CHConfig_.HOST << ":" << CHConfig_.PORT;
        request.setOpt(new curlpp::options::Url(std::string(endpoint.str())));
        request.setOpt(new curlpp::options::WriteStream(&response));
        // request.setOpt(new curlpp::options::SslEngineDefault());
        request.perform();
        uint64_t response_code = curlpp::infos::ResponseCode::get(request);
        std::string response_body = response.str();
        LOG_DEBUG << "Clickhouse's response: code - " << response_code << ", " << response_body;
        return response_code == 200 && response_body.find("Ok.") != std::string::npos;
    } catch (curlpp::RuntimeError& e) {
        LOG_DEBUG << e.what();
    } catch (curlpp::LogicError& e) {
        LOG_DEBUG << e.what();
    }

    return false;
}

ClickHouseConnectorConfig Logbroker::GetClickHouseConnectorConfig() {
    return {
        .HOST = GetEnvOrDefault("LOGBROKER_CH_HOST", "localhost"),
        .USER = GetEnvOrDefault("LOGBROKER_CH_USER"),
        .PASSWORD = GetEnvOrDefault("LOGBROKER_CH_PASSWORD"),
        .PORT = std::stoi(GetEnvOrDefault("LOGBROKER_CH_PORT", "80")),
        .CERT_PATH = GetEnvOrDefault("LOGBROKER_CH_CERT_PATH")
    };
}

trantor::LogStream& operator<<(trantor::LogStream& out, 
                               const Logbroker::ClickHouseConnectorConfig& chConfig) 
{
    return out << "HOST: " << chConfig.HOST << '\n'
               << "USER: " << chConfig.USER << '\n'
               << "PASSWORD: " << chConfig.PASSWORD << '\n'
               << "PORT: " << chConfig.PORT << '\n'
               << "CERT_PATH: " << chConfig.CERT_PATH;
}
