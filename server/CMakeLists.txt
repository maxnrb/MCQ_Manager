cmake_minimum_required(VERSION 3.7)
project(server)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/include/soci)
include_directories(/usr/include/soci/mysql)
include_directories(/usr/include/mysql)

#add_executable(Projet main.cpp HelloHandler.cpp HelloHandler.h headers/CORS.h Database.cpp Database.h utils/sha1.cpp utils/sha1.h utils/Utils.cpp utils/Utils.h headers/Authorization.h utils/base64.cpp utils/base64.h)

file (GLOB_RECURSE Project_files src/*.cpp src/*.h)

add_executable(server ${Project_files} src/model/Test.cpp src/model/Test.h src/model/Question.cpp src/model/Question.h src/model/Answer.cpp src/model/Answer.h)

target_link_libraries(server pistache)
target_link_libraries(server soci_core soci_mysql)
