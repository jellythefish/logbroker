#include "write_log.h"

using namespace Logbroker;

WriteLogHandler::WriteLogHandler(PersistentLoggerPtr persistentLoggerPtr)
    : PersistentLoggerPtr_(persistentLoggerPtr) 
{
}

void WriteLogHandler::HandleWriteLog(const HttpRequestPtr& req,
                                     Callback&& callback) const
{
    LOG_DEBUG << req->getMethodString() << " " << req->getPath()
            << " from " << req->getHeaders().at("host");
    auto responce = HttpResponse::newHttpResponse();
    responce->setContentTypeCode(CT_TEXT_PLAIN);
    bool result = PersistentLoggerPtr_->Log(std::string(req->getBody()).c_str());
    if (result) {
        responce->setStatusCode(k200OK);
    } else {
        responce->setStatusCode(k503ServiceUnavailable);
        responce->setBody("Something went wrong");
    }
    callback(responce);
}
