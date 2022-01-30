#pragma once

#include <drogon/HttpController.h>

#include <libs/clickhouse_connector.h>

using namespace drogon;

namespace Logbroker {

    using Callback = std::function<void(const HttpResponsePtr&)>;

    class HealthCheckerReqHandler: public drogon::HttpController<HealthCheckerReqHandler, false> {
        public:        
            METHOD_LIST_BEGIN
                ADD_METHOD_TO(HealthCheckerReqHandler::HandleHealthCheck, "/healthcheck", Get);
            METHOD_LIST_END

        HealthCheckerReqHandler(ClickHouseConnectorPtr clickHouseConnectorPtr);

        void HandleHealthCheck(const HttpRequestPtr& req, Callback&& callback) const;

        private:
            ClickHouseConnectorPtr ClickHouseConnectorPtr_;
    };
} // namespace Logbroker
