#pragma once

#include "clickhouse_connector.h"

#include <drogon/drogon.h>

#include <fstream>
#include <filesystem>
#include <vector>
#include <mutex>
#include <memory>

#include <stdint.h>

using namespace std::literals::chrono_literals;

namespace Logbroker {
    // TODO: enable configuring from env
    struct PersistentLoggerConfig {
        uint64_t MaxRowsInLog = 10;
        std::string LogFilename = "persistent.log";
        std::chrono::milliseconds CommitPeriodMs = 30000ms;
        uint64_t RetriesCount = 3;
    };

    class PersistentLogger {
        public:
            PersistentLogger(const PersistentLoggerConfig& config,
                             ClickHouseConnectorPtr clickHouseConnectorPtr);

            bool Log(const std::string& logEntry);

            bool TrySendClickHouseLogEntries(const ClickHouseLogEntries& logEntries) const;

            void TryCommit();

            ~PersistentLogger();
        private:
            ClickHouseConnectorPtr ClickHouseConnectorPtr_;

            PersistentLoggerConfig Config_;
            uint64_t TotalEntries_; // Total commited entries

            std::ofstream LogFile_;
            std::vector<std::string> LogBuffer_;
            std::mutex LogMutex_;

            std::shared_ptr<std::jthread> PosterThread_;
    };

    using PersistentLoggerPtr = std::shared_ptr<PersistentLogger>;
}

trantor::LogStream& operator<<(trantor::LogStream& out,
                               const Logbroker::PersistentLoggerConfig& config);
