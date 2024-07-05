add_library(logic
    actions.c
    actions.h
    logic.h
    logic.c
    session_data.h
)

target_include_directories(logic PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/networking
    ${CMAKE_SOURCE_DIR}/database
    ${JSON-C_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    )
target_link_libraries(logic PRIVATE ${CURL_LIBRARIES} ${JSON-C_LIBRARIES})
target_compile_options(logic PRIVATE -g -Wall)
