# Create a library for the folder
add_library(database
    accessor.h
    accessor.c
    token.h
    token.c
    db_structs.h
    web-utils.h
    web-utils.c
)

# Include it
target_include_directories(database PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}) 
target_link_libraries(database PRIVATE ${CURL_LIBRARIES} ${JSON-C_LIBRARIES})
target_compile_options(database PRIVATE -g -Wall)