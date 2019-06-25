cmake_minimum_required(VERSION 3.7)
set(CMAKE_AUTOMOC ON)
project(server)

set(CMAKE_CXX_STANDARD 14)
include_directories(/usr/include/soci)
include_directories(/usr/include/soci/mysql)
include_directories(/usr/include/mysql)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)

file (GLOB_RECURSE Project_files src/*.cpp src/*.h)

add_executable(server ${Project_files})

target_link_libraries(server pistache)
target_link_libraries(server soci_core soci_mysql)
target_link_libraries(server Qt5::Widgets)
target_link_libraries(server Qt5::Network)