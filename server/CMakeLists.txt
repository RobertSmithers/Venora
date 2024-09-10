cmake_minimum_required(VERSION 3.14)
project(VenoraServer)

# If missing this library, install with
# `apt-get install libpq-dev postgresql-server-dev-all`
# find_package(PostgreSQL REQUIRED)
# `apt-get install libcurl4-openssl-dev libjson-c-dev`
find_package(CURL REQUIRED)

find_package(OpenSSL REQUIRED)

# Find JSON-C package
find_path(JSONC_INCLUDE_DIR json-c/json.h PATHS /usr/include/json-c)
find_library(JSONC_LIBRARY NAMES json-c PATHS /usr/lib/x86_64-linux-gnu)

if (JSONC_INCLUDE_DIR AND JSONC_LIBRARY)
    set(JSON-C_FOUND TRUE)
    set(JSON-C_INCLUDE_DIRS ${JSONC_INCLUDE_DIR})
    set(JSON-C_LIBRARIES ${JSONC_LIBRARY})
else()
    set(JSON-C_FOUND FALSE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/logic ${CMAKE_SOURCE_DIR}/database ${CURL_INCLUDE_DIRS} ${JSON-C_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})


add_subdirectory(logic)
add_subdirectory(networking)
add_subdirectory(database)
add_subdirectory(tests)

add_executable(server server.c log.c)
enable_testing()

target_link_libraries(server PRIVATE logic networking database ${JSON-C_LIBRARIES} ${CURL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)

target_compile_options(server PRIVATE -g -Wall)
