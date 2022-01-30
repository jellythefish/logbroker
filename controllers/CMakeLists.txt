cmake_minimum_required(VERSION 3.5)
project(logbroker_controllers CXX)

add_executable(${PROJECT_NAME}
                health_check.cpp
                show_create_table.cpp
                write_log.cpp)
