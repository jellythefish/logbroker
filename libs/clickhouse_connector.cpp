#include "clickhouse_connector.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include <cassert>
#include <stdint.h>

using namespace Logbroker;

ClickHouseConnectorConfig Logbroker::GetClickHouseConnectorConfig() {
    return {
        .Host = GetEnvOrDefault("LOGBROKER_CH_HOST", "localhost"),
        .User = GetEnvOrDefault("LOGBROKER_CH_USER"),
        .Password = GetEnvOrDefault("LOGBROKER_CH_PASSWORD"),
        .Port = std::stoi(GetEnvOrDefault("LOGBROKER_CH_PORT", "80")),
        .CertPath = GetEnvOrDefault("LOGBROKER_CH_CERT_PATH")
    };
}

ClickHouseConnector::ClickHouseConnector(ClickHouseConnectorConfig chConfig) 
    : CHConfig_(chConfig)
{
}

ClickHouseResponse ClickHouseConnector::ClickHouseIsOK() const {
    ClickHouseRequest request{
        .Type = ClickHouseRequestType::GET,
        .UrlParams = "/?query=SELECT%20%27wassup%2C%20clickhouse%20in%20the%20building%27",
    };
    return SendRequest(request);
}

ClickHouseResponse ClickHouseConnector::ShowCreateTable(const std::string& tableName) const {
    ClickHouseRequest request{
        .Type = ClickHouseRequestType::GET,
        .UrlParams = "/?query=SHOW%20CREATE%20TABLE%20%22" + tableName + "%22",
    };
    return SendRequest(request);
}

ClickHouseLogEntries ClickHouseConnector::ParseRawLogEntries(const std::vector<std::string>& logEntries) const {
    ClickHouseLogEntries entries;
    for (const auto& logEntry : logEntries) {
        Json::Value logs;
        Json::Reader reader;
        if (!reader.parse(logEntry.c_str(), logs)) {
            LOG_DEBUG << "Failed to parse JSON log entry" << reader.getFormattedErrorMessages();
            continue;
        }
        for (const auto& log : logs) {
            ClickHouseQueryFormat queryFormat;
            if (log["format"].asString() == "json") {
                queryFormat = ClickHouseQueryFormat::JSONEachRow;
            } else if (log["format"].asString() == "list") {
                queryFormat = ClickHouseQueryFormat::CSV;
            } else {
                LOG_ERROR << "Unknown log entry format: " + log["format"].asString();
                continue;
            }
            std::vector<std::string> rows;
            for (const auto& row : log["rows"]) {
                // format CSV rows in-place, JSON rows come as is as they are valid
                if (queryFormat == ClickHouseQueryFormat::CSV) {
                    std::stringstream csvRow;
                    std::string sep;
                    for (const auto& elem : row) {
                        csvRow << sep << elem;
                        sep = ",";
                    }
                    rows.push_back(csvRow.str());
                } else {
                    Json::StreamWriterBuilder builder;
                    builder["indentation"] = ""; 
                    rows.push_back(Json::writeString(builder, row));
                }
            }
            entries.emplace_back(ClickHouseLogEntry{
                .QueryFormat = queryFormat,
                .TableName = log["table_name"].asString(),
                .Rows = rows
            });
        }
    }
    // LOG_DEBUG << "parsed entries:\n" << entries;
    return entries;
}

ClickHouseResponse ClickHouseConnector::SendRequest(const ClickHouseRequest& chRequest) const {
    std::string errorMessage;
    try {
        LOG_DEBUG << "Sending "<< chRequest.Type << " request to ClickHouse server: " 
                  << chRequest.UrlParams;
                //   << " with body: \n" << chRequest.Body;
        curlpp::Cleanup myCleanup;
        curlpp::Easy request;
        std::stringstream response;
        std::stringstream endpoint;

        endpoint << "http://" << CHConfig_.Host << ":" << CHConfig_.Port << chRequest.UrlParams;
        request.setOpt(new curlpp::options::Url(std::string(endpoint.str())));
        if (chRequest.Type == ClickHouseRequestType::POST) {
            request.setOpt(new curlpp::options::PostFields(chRequest.Body));
        }
        request.setOpt(new curlpp::options::WriteStream(&response));
        // TODO: configure SSL later
        // request.setOpt(new curlpp::options::SslEngineDefault());
        request.perform();
        uint64_t response_code = curlpp::infos::ResponseCode::get(request);
        std::string response_body = response.str();
        return {response_code, response_body};
    } catch (curlpp::RuntimeError& e) {
        errorMessage = e.what();
        LOG_ERROR << errorMessage;
    } catch (curlpp::LogicError& e) {
        errorMessage = e.what();
        LOG_ERROR << errorMessage;
    } catch (const std::exception& e) {
        errorMessage = e.what();
        LOG_ERROR << errorMessage;
    }
    return {500, errorMessage};
}

ClickHouseRequests ClickHouseConnector::BuildClickHouseRequests(const ClickHouseLogEntries& logEntries) const {
    // pairs of table names and requests
    std::unordered_map<std::string, ClickHouseLogEntries> tableRequests;
    for (const auto& entry : logEntries) {
        tableRequests[entry.TableName].push_back(entry);
    }
    ClickHouseRequests requests;
    for (const auto& [tableName, entries] : tableRequests) {
        assert(entries.size() > 0);
        ClickHouseRequest request;
        request.Type = ClickHouseRequestType::POST;
        std::string queryFormat = entries[0].QueryFormat == ClickHouseQueryFormat::CSV ?
                                  "CSV" : "JSONEachRow";
        request.UrlParams = "/?query=INSERT%20INTO%20" + tableName + "%20FORMAT%20" + queryFormat;
        std::string body;
        std::string sep;
        for (const auto& entry : entries) {
            for (const auto& row : entry.Rows) {
                body += sep + row;
                sep = "\n";
            }
        }
        request.Body = body;
        requests.push_back(request);
    }
    return requests;
}

bool ClickHouseConnector::MakeRequests(const ClickHouseRequests& requests) const {
    bool allRequestsSucceded = true;
    for (const auto& request : requests) {
        auto response = SendRequest(request);
        if (response.StatusCode != 200) {
            allRequestsSucceded = false;
            continue;
        }
        std::stringstream message;
        message << "ClickHouse Response is (" << response.StatusCode << "): " << response.Body;
        if (response.StatusCode != 200) {
            allRequestsSucceded = false;
            LOG_ERROR << message.str();
        } else {
            LOG_DEBUG << message.str();
        }
    }
    return allRequestsSucceded;
}

ClickHouseLogEntries ClickHouseConnector::FilterEntries(const ClickHouseLogEntries& entries,
                                                        const ClickHouseQueryFormat& format) const
{
    ClickHouseLogEntries filteredEntries;
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(filteredEntries),
        [&](const ClickHouseLogEntry& entry) { 
        return entry.QueryFormat == format;
    });
    return filteredEntries;
}

bool ClickHouseConnector::SendClickHouseLogEntries(const ClickHouseLogEntries& entries) const {
    if (entries.empty()) {
        LOG_DEBUG << "No entries to send";
        return true;
    }
    return MakeRequests(BuildClickHouseRequests(entries));
}


// #################################################################################################

trantor::LogStream& operator<<(trantor::LogStream& out, 
                               const Logbroker::ClickHouseConnectorConfig& config) 
{
    return out << "Host: " << config.Host << '\n'
               << "User: " << config.User << '\n'
               << "Password: " << config.Password << '\n'
               << "Port: " << config.Port << '\n'
               << "CertPath: " << config.CertPath;
}

trantor::LogStream& operator<<(trantor::LogStream& out,
                               const Logbroker::ClickHouseLogEntry& entry)
{
    out << "{QueryFormat: " << entry.QueryFormat << '\n'
        << "TableName: " << entry.TableName << '\n'
        << "Rows: \n";
    std::string sep;
    for (const auto& row : entry.Rows) {
        out << sep << row;
        sep = "\n";
    }
    return out;
}

trantor::LogStream& operator<<(trantor::LogStream& out,
                               const Logbroker::ClickHouseLogEntries& entries)
{
    std::string sep;
    for (const auto& entry : entries) {
        out << sep << entry;
        sep = ", \n";
    }
    return out;
}
