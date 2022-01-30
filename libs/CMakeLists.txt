cmake_minimum_required(VERSION 3.5)

add_library(logbroker_libs clickhouse_connector.cpp)

target_link_libraries(logbroker_libs PUBLIC curlpp)
target_link_libraries(logbroker_libs PUBLIC Drogon::Drogon)
