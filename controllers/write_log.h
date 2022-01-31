#pragma once

#include <drogon/HttpController.h>

#include <libs/persistent_logger.h>

using namespace drogon;

namespace Logbroker {

    using Callback = std::function<void(const HttpResponsePtr&)>;

    class WriteLogHandler: public drogon::HttpController<WriteLogHandler, false> {
        public:
            METHOD_LIST_BEGIN
                ADD_METHOD_TO(WriteLogHandler::HandleWriteLog, "/write_log", Post);
            METHOD_LIST_END

        WriteLogHandler(PersistentLoggerPtr persistentLoggerPtr);

        void HandleWriteLog(const HttpRequestPtr& req, Callback&& callback) const;

        private:
            PersistentLoggerPtr PersistentLoggerPtr_;
    };
} // namespace Logbroker
