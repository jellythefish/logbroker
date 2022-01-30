#pragma once

#include "helpers.h"

#include <drogon/drogon.h>

#include <memory>

namespace Logbroker {

    struct ClickHouseConnectorConfig {
        std::string HOST;
        std::string USER;
        std::string PASSWORD;
        int PORT;
        std::string CERT_PATH;
    };

    ClickHouseConnectorConfig GetClickHouseConnectorConfig();

    class ClickHouseConnector {
        public:
            ClickHouseConnector(ClickHouseConnectorConfig chCongig);

            bool ClickHouseIsOK() const;
        private:
            ClickHouseConnectorConfig CHConfig_;
    };

    using ClickHouseConnectorPtr = std::shared_ptr<ClickHouseConnector>;
}

trantor::LogStream& operator<<(trantor::LogStream& out, const Logbroker::ClickHouseConnectorConfig& chConfig);
