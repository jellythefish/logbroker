#include "persistent_logger.h"
#include "clickhouse_connector.h"
#include <fstream>
#include <mutex>
#include <stdint.h>

#include <thread>

using namespace Logbroker;

namespace fs = std::filesystem;

bool PersistentLogger::TrySendClickHouseLogEntries(const ClickHouseLogEntries& logEntries) const {
    bool succeded = ClickHouseConnectorPtr_->SendClickHouseLogEntries(logEntries);
    LOG_DEBUG << "try is " << succeded;
    uint64_t retriesMade = 0;
    while (!succeded && retriesMade < Config_.RetriesCount) {
        LOG_ERROR << "Some requests are failed: retrying...";
        succeded = ClickHouseConnectorPtr_->SendClickHouseLogEntries(logEntries);
        std::this_thread::sleep_for(300ms);
        ++retriesMade;
    }
    return succeded;
}

void PersistentLogger::TryCommit() {
    if (LogBuffer_.empty()) {
        LOG_DEBUG << "Log is empty, nothing to commit";
        return;
    }
    ClickHouseLogEntries logEntries = ClickHouseConnectorPtr_->ParseRawLogEntries(LogBuffer_);
    ClickHouseLogEntries jsonEntries = ClickHouseConnectorPtr_->FilterEntries(
        logEntries, ClickHouseQueryFormat::JSONEachRow
    );
    ClickHouseLogEntries csvEntries = ClickHouseConnectorPtr_->FilterEntries(
        logEntries, ClickHouseQueryFormat::CSV
    );

    // if one of trySend is failed and another is succeded than on next TryCommit iteration
    // it will trySend the succeded entries again, but the duplicates should be ommited via
    // database considering the unique keys
    bool jsonSucceded = TrySendClickHouseLogEntries(jsonEntries);
    bool csvSucceded = TrySendClickHouseLogEntries(csvEntries);
    if (jsonSucceded && csvSucceded) {
        LOG_DEBUG << "Commited " << LogBuffer_.size() << " log entries to ClickHouse";
        // clearing log state
        LogBuffer_.clear();
        fs::resize_file(fs::path(Config_.LogFilename), 0);
    }
}

PersistentLogger::PersistentLogger(const PersistentLoggerConfig& config,
                                   ClickHouseConnectorPtr clickHouseConnectorPtr)
    : ClickHouseConnectorPtr_(clickHouseConnectorPtr)
    , LogFile_(config.LogFilename, std::ios_base::app)
{
    // initialize in-memory log buffer
    std::ifstream logFile(config.LogFilename);
    std::string str;
    while (std::getline(logFile, str)) {
        LogBuffer_.push_back(str);
    }
    LOG_DEBUG << "Buffer initialized with " << LogBuffer_.size() << " rows";

    PosterThread_ = std::make_shared<std::jthread>([&](std::stop_token stopToken) {
        uint64_t value = 0;
        while (!stopToken.stop_requested()) {
            std::this_thread::sleep_for(Config_.CommitPeriodMs);
            std::lock_guard<std::mutex> guard(LogMutex_);
            TryCommit();
        }
    });
}

bool PersistentLogger::Log(const std::string& logEntry) {
    try {
        std::lock_guard<std::mutex> guard(LogMutex_);
        LogFile_ << logEntry << '\n' << std::flush;
        LogBuffer_.push_back(logEntry);
        
        if (LogBuffer_.size() >= Config_.MaxRowsInLog) {
            LOG_DEBUG << "Log buffer is max capacity, trying to commit...";
            TryCommit();
        }
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR << "Error occured while logging new entry: " << e.what();
    }
    return false;
}

PersistentLogger::~PersistentLogger() {
    PosterThread_->request_stop();
    PosterThread_->join();
    LogFile_.flush();
    LogFile_.close(); 
}

// ################################################################################################

trantor::LogStream& operator<<(trantor::LogStream& out, 
                               const Logbroker::PersistentLoggerConfig& config)
{
    return out << "MaxRowsInLog: " << config.MaxRowsInLog << '\n'
               << "LogFilename: " << config.LogFilename << '\n'
               << "CommitPeriodMs: " << config.CommitPeriodMs.count() << '\n'
               << "RetriesCount: " << config.RetriesCount;
}
