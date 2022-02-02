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
    auto response = HttpResponse::newHttpResponse();
    response->setContentTypeCode(CT_TEXT_PLAIN);
    auto chResponse = ClickHouseConnectorPtr_->ClickHouseIsOK();
    if (chResponse.StatusCode == 200) {
        response->setStatusCode(k200OK);
        response->setBody(chResponse.Body);
        LOG_DEBUG << "ClickHouse is OK";
    } else {
        response->setStatusCode(k503ServiceUnavailable);
        response->setBody("ClickHouse is not responding");
        LOG_ERROR << "ClickHouse is not OK: " << chResponse.Body;
    }
    callback(response);
}
