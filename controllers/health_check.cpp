#include "health_check.h"

using namespace Logbroker;

HealthCheckerReqHandler::HealthCheckerReqHandler(ClickHouseConnectorPtr clickHouseConnectorPtr)
    : ClickHouseConnectorPtr_(clickHouseConnectorPtr) 
{
}

void HealthCheckerReqHandler::HandleHealthCheck(const HttpRequestPtr& req,
                                                Callback&& callback) const
{
    LOG_DEBUG << "GET /healthcheck from " << req->getHeaders().at("host");
    auto responce = HttpResponse::newHttpResponse();
    if (ClickHouseConnectorPtr_->ClickHouseIsOK()) {
        responce->setContentTypeCode(CT_NONE);
        responce->setStatusCode(k200OK);
        LOG_DEBUG << "ClickHouse is OK";
    } else {
        responce->setContentTypeCode(CT_TEXT_PLAIN);
        responce->setStatusCode(k503ServiceUnavailable);
        responce->setBody("ClickHouse is not responding.");
        LOG_DEBUG << "ClickHouse is not OK";
    }
    callback(responce);
}
