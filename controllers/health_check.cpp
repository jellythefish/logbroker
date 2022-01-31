#include "health_check.h"

using namespace Logbroker;

HealthCheckerReqHandler::HealthCheckerReqHandler(ClickHouseConnectorPtr clickHouseConnectorPtr)
    : ClickHouseConnectorPtr_(clickHouseConnectorPtr) 
{
}

void HealthCheckerReqHandler::HandleHealthCheck(const HttpRequestPtr& req,
                                                Callback&& callback) const
{
    LOG_DEBUG << req->getMethodString() << " " << req->getPath()
              << " from " << req->getHeaders().at("host");
    auto responce = HttpResponse::newHttpResponse();
    responce->setContentTypeCode(CT_TEXT_PLAIN);
    auto chResponse = ClickHouseConnectorPtr_->ClickHouseIsOK();
    if (chResponse && chResponse->StatusCode == 200) {
        responce->setStatusCode(k200OK);
        responce->setBody(chResponse->Body);
        LOG_DEBUG << "ClickHouse is OK";
    } else {
        responce->setStatusCode(k503ServiceUnavailable);
        responce->setBody("ClickHouse is not responding");
        LOG_ERROR << "ClickHouse is not OK: " << chResponse->Body;
    }
    callback(responce);
}
