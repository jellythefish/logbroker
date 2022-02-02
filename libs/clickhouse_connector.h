#pragma once

#include "helpers.h"

#include <drogon/drogon.h>

#include <memory>
#include <stdint.h>
#include <vector>

namespace Logbroker {

    struct ClickHouseConnectorConfig {
        std::string Host;
        std::string User;
        std::string Password;
        int Port;
        std::string CertPath;
    };

    enum ClickHouseQueryFormat {
        JSONEachRow = 1,
        CSV = 2
    };
    enum ClickHouseRequestType {
        GET = 1,
        POST = 2
    };

    struct ClickHouseLogEntry {
        ClickHouseQueryFormat QueryFormat;
        std::string TableName;
        std::vector<std::string> Rows;
    };
    using ClickHouseLogEntries = std::vector<ClickHouseLogEntry>;
    
    struct ClickHouseRequest {
        ClickHouseRequestType Type;
        std::string UrlParams;
        std::string Body;
    };
    using ClickHouseRequests = std::vector<ClickHouseRequest>;
    struct ClickHouseResponse {
        uint64_t StatusCode = 419;
        std::string Body = "";
    };

    ClickHouseConnectorConfig GetClickHouseConnectorConfig();

    class ClickHouseConnector {
        public:
            ClickHouseConnector(ClickHouseConnectorConfig chCongig);

            ClickHouseResponse ClickHouseIsOK() const;

            ClickHouseResponse ShowCreateTable(const std::string& tableName) const;

            ClickHouseResponse SendRequest(const ClickHouseRequest& chRequest) const;

            ClickHouseLogEntries ParseRawLogEntries(const std::vector<std::string>& logEntries) const;

            ClickHouseLogEntries FilterEntries(const ClickHouseLogEntries& entries,
                                               const ClickHouseQueryFormat& format) const;

            ClickHouseRequests BuildClickHouseRequests(const ClickHouseLogEntries&) const;

            bool SendClickHouseLogEntries(const ClickHouseLogEntries& queries) const;

            bool MakeRequests(const ClickHouseRequests& requests) const;

        private:
            ClickHouseConnectorConfig CHConfig_;
    };

    using ClickHouseConnectorPtr = std::shared_ptr<ClickHouseConnector>;
}

// ################################################################################################

trantor::LogStream& operator<<(trantor::LogStream& out,
                               const Logbroker::ClickHouseConnectorConfig& config);

trantor::LogStream& operator<<(trantor::LogStream& out,
                               const Logbroker::ClickHouseLogEntry& entry);

trantor::LogStream& operator<<(trantor::LogStream& out,
                               const Logbroker::ClickHouseLogEntries& entries);
