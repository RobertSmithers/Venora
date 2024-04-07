cmake_minimum_required(VERSION 3.14)
project(VenoraServer)

# If missing this library, install with
# `apt-get install libpq-dev postgresql-server-dev-all`
find_package(PostgreSQL REQUIRED)

add_subdirectory(logic)
add_subdirectory(networking)
add_subdirectory(database)

add_executable(server server.c)

target_include_directories(server PRIVATE ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(server PRIVATE logic networking database ${PostgreSQL_LIBRARIES})

target_compile_options(server PRIVATE -g -Wall)