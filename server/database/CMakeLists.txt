# Create a library for the folder
add_library(database
    accessor.h
    accessor.c
    token.h
    token.c
    db_structs.h
)

# Include it
target_include_directories(database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PostgreSQL_INCLUDE_DIRS}) 
target_compile_options(networking PRIVATE -g -Wall)