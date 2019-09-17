cmake_minimum_required(VERSION 2.8.1)
project(client)


set (SOURCE_DIR             ${PROJECT_SOURCE_DIR}/source)
set (INCLUDE_DIR            ${PROJECT_SOURCE_DIR}/source)

set(CMAKE_CXX_STANDARD 11)

include_directories(${INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -Wextra -ffunction-sections -O0")




#client
add_executable(client
        ${SOURCE_DIR}/cJSON.c
        ${SOURCE_DIR}/myLog.cpp
        ${SOURCE_DIR}/message.cpp
        ${SOURCE_DIR}/strategy.cpp
        ${SOURCE_DIR}/map.cpp
        ${SOURCE_DIR}/socketClient.cpp
        ${SOURCE_DIR}/main.cpp
)
#target_include_directories(client)

#server

# Library dependent

#target_link_libraries(client ${PROTOBUF_LIBRARIES})

