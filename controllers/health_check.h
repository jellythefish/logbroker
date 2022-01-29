#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace Logbroker {

    using Callback = std::function<void(const HttpResponsePtr&)>;

    class HealthChecker: public drogon::HttpController<HealthChecker> {
        public:
            METHOD_LIST_BEGIN
                ADD_METHOD_TO(HealthChecker::Check, "/healthcheck", Get);
            METHOD_LIST_END

        void Check(const HttpRequestPtr& req, Callback&& callback) const;
    };
} // namespace Logbroker
