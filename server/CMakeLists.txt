cmake_minimum_required(VERSION 3.10)
project(VenoraServer)

add_subdirectory(logic)
add_subdirectory(networking)
add_subdirectory(database)

add_executable(server server.c)

target_link_libraries(server PRIVATE logic networking database)
target_compile_options(server PRIVATE -g)