#include "health_check.h"

using namespace Logbroker;

void HealthChecker::Check(const HttpRequestPtr& req, Callback&& callback) const {
    LOG_DEBUG << "GET /healthcheck from " << req->getHeaders().at("host") << '\n';
    auto responce = HttpResponse::newHttpResponse();
    responce->setStatusCode(k200OK);
    responce->setContentTypeCode(CT_NONE);
    callback(responce);
}
