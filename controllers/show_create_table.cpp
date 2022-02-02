#include "show_create_table.h"

using namespace Logbroker;

ShowCreateTableHandler::ShowCreateTableHandler(ClickHouseConnectorPtr clickHouseConnectorPtr)
    : ClickHouseConnectorPtr_(clickHouseConnectorPtr) 
{
}

void ShowCreateTableHandler::HandleShowRequest(const HttpRequestPtr& req, Callback&& callback) const {
    LOG_DEBUG << req->getMethodString() << " " << req->getPath() << "?" << req->getQuery() 
              << " from " << req->getHeaders().at("host");
    auto response = HttpResponse::newHttpResponse();
    response->setContentTypeCode(CT_TEXT_PLAIN);
    auto tableName = req->getParameter("table_name");
    if (tableName.empty()) {
        std::string errorMessage = "Show create table request failed: table name is empty";
        response->setStatusCode(k400BadRequest);
        response->setBody(errorMessage);
        LOG_ERROR << errorMessage;
        callback(response);
        return;
    }
    auto chResponse = ClickHouseConnectorPtr_->ShowCreateTable(tableName);
    if (chResponse.StatusCode == 200) {
        response->setStatusCode(k200OK);
        response->setBody(chResponse.Body);
    } else {
        response->setStatusCode(k503ServiceUnavailable);
        response->setBody("Show create table request failed: " + chResponse.Body);
        LOG_ERROR << "Show create table request failed" << chResponse.Body;
    }
    callback(response);
}

// TODO refactor later
void ShowCreateTableHandler::HandleShowTable(const HttpRequestPtr& req, Callback&& callback) const {
    LOG_DEBUG << req->getMethodString() << " " << req->getPath() << "?" << req->getQuery() 
              << " from " << req->getHeaders().at("host");
    auto response = HttpResponse::newHttpResponse();
    response->setContentTypeCode(CT_TEXT_PLAIN);
    auto tableName = req->getParameter("table_name");
    if (tableName.empty()) {
        std::string errorMessage = "Show table request failed: table name is empty";
        response->setStatusCode(k400BadRequest);
        response->setBody(errorMessage);
        LOG_ERROR << errorMessage;
        callback(response);
        return;
    }
    auto chResponse = ClickHouseConnectorPtr_->ShowTable(tableName);
    if (chResponse.StatusCode == 200) {
        response->setStatusCode(k200OK);
        response->setBody(chResponse.Body);
    } else {
        response->setStatusCode(k503ServiceUnavailable);
        response->setBody("Show table request failed: " + chResponse.Body);
        LOG_ERROR << "Show table request failed" << chResponse.Body;
    }
    callback(response);
}
