#include "show_create_table.h"

using namespace Logbroker;

ShowCreateTableHandler::ShowCreateTableHandler(ClickHouseConnectorPtr clickHouseConnectorPtr)
    : ClickHouseConnectorPtr_(clickHouseConnectorPtr) 
{
}

void ShowCreateTableHandler::HandleShowRequest(const HttpRequestPtr& req, Callback&& callback) const {
    LOG_DEBUG << req->getMethodString() << " " << req->getPath() << "?" << req->getQuery() 
              << " from " << req->getHeaders().at("host");
    auto responce = HttpResponse::newHttpResponse();
    responce->setContentTypeCode(CT_TEXT_PLAIN);
    auto tableName = req->getParameter("table_name");
    if (tableName.empty()) {
        std::string errorMessage = "Show create table request failed: table name is empty";
        responce->setStatusCode(k400BadRequest);
        responce->setBody(errorMessage);
        LOG_ERROR << errorMessage;
        callback(responce);
        return;
    }
    auto chResponse = ClickHouseConnectorPtr_->ShowCreateTable(tableName);
    if (chResponse && chResponse->StatusCode == 200) {
        responce->setStatusCode(k200OK);
        responce->setBody(chResponse->Body);
    } else {
        responce->setStatusCode(k503ServiceUnavailable);
        responce->setBody("Show create table request failed: " + chResponse->Body);
        LOG_ERROR << "Show create table request failed" << chResponse->Body;
    }
    callback(responce);
}
