#include <drogon/drogon.h>

#include <controllers/health_check.h>
#include <controllers/write_log.h>
#include <controllers/show_create_table.h>

#include <libs/clickhouse_connector.h>
#include <libs/persistent_logger.h>

#include <cstdlib>
#include <ostream>

using namespace Logbroker;

int main() {
    // TODO: fix output to file in the beginning
    ClickHouseConnectorConfig chConnectorConfig = GetClickHouseConnectorConfig();
    LOG_DEBUG << "ClickHouse config is: \n" << chConnectorConfig;
    PersistentLoggerConfig persistentLoggerConfig;
    LOG_DEBUG << "Persistent logger config is: \n" << persistentLoggerConfig;

    auto clickHouseConnectorPtr = std::make_shared<ClickHouseConnector>(chConnectorConfig);
    auto persistentLoggerPtr = std::make_shared<PersistentLogger>(
        persistentLoggerConfig,
        clickHouseConnectorPtr
    );

    auto healthCheckControllerPtr = std::make_shared<HealthCheckerReqHandler>(clickHouseConnectorPtr);
    auto showCreateTableControllerPtr = std::make_shared<ShowCreateTableHandler>(clickHouseConnectorPtr);
    auto writeLogControllerPtr = std::make_shared<WriteLogHandler>(persistentLoggerPtr);

    // flush LOG_DEBUG macroses to stdout redirected to server.log manually before app runs
    fflush(stdout); 

    drogon::app().registerController(healthCheckControllerPtr)
                 .registerController(showCreateTableControllerPtr)
                 .registerController(writeLogControllerPtr);
    drogon::app().loadConfigFile("../config.json");
    drogon::app().addListener("0.0.0.0", 80);
    drogon::app().run(); // the method will block in the internal event loop

    return 0;
}
