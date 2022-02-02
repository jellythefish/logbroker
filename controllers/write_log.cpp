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
    auto response = HttpResponse::newHttpResponse();
    response->setContentTypeCode(CT_TEXT_PLAIN);
    // TODO: answer 404 if talbe not exists
    bool result = PersistentLoggerPtr_->Log(std::string(req->getBody()).c_str());
    if (result) {
        response->setStatusCode(k200OK);
    } else {
        response->setStatusCode(k503ServiceUnavailable);
        response->setBody("Something went wrong");
    }
    callback(response);
}
