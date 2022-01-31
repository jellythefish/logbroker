cmake_minimum_required(VERSION 3.5)

add_library(logbroker_libs 
            clickhouse_connector.cpp
            persistent_logger.cpp
            helpers.cpp)

target_link_libraries(logbroker_libs PUBLIC
                      curlpp
                      Drogon::Drogon)
