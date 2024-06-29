# Create a library for the folder
add_library(logic
    actions.c
    actions.h
    logic.h
    logic.c
    session_data.h
)

# Include it
target_include_directories(logic PUBLIC ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${PostgreSQL_INCLUDE_DIRS} networking)
target_compile_options(logic PRIVATE -g -Wall)
