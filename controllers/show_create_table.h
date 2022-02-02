#pragma once

#include <drogon/HttpController.h>

#include <libs/clickhouse_connector.h>

using namespace drogon;

namespace Logbroker {

    using Callback = std::function<void(const HttpResponsePtr&)>;

    class ShowCreateTableHandler: public drogon::HttpController<ShowCreateTableHandler, false> {
        public:        
            METHOD_LIST_BEGIN
                ADD_METHOD_TO(ShowCreateTableHandler::HandleShowRequest, "/show_create_table", Get);
                ADD_METHOD_TO(ShowCreateTableHandler::HandleShowTable, "/show_table", Get);
            METHOD_LIST_END

        ShowCreateTableHandler(ClickHouseConnectorPtr clickHouseConnectorPtr);

        void HandleShowRequest(const HttpRequestPtr& req, Callback&& callback) const;
        
        void HandleShowTable(const HttpRequestPtr& req, Callback&& callback) const;

        private:
            ClickHouseConnectorPtr ClickHouseConnectorPtr_;
    };
} // namespace Logbroker
