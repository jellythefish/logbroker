#include <drogon/drogon.h>

#include <controllers/health_check.h>
#include <libs/clickhouse_connector.h>

#include <cstdlib>
#include <ostream>

using namespace Logbroker;

int main() {
    ClickHouseConnectorConfig chConnectorConfig = GetClickHouseConnectorConfig();
    LOG_DEBUG << "ClickHouse config is: \n" << chConnectorConfig;

    auto clickHouseConnectorPtr = std::make_shared<ClickHouseConnector>(chConnectorConfig);
    auto healthCheckControllerPtr = std::make_shared<HealthCheckerReqHandler>(clickHouseConnectorPtr);

    drogon::app().registerController(healthCheckControllerPtr);
    drogon::app().addListener("0.0.0.0", 80);
    drogon::app().loadConfigFile("../config.json");
    drogon::app().run(); // the method will block in the internal event loop

    return 0;
}
