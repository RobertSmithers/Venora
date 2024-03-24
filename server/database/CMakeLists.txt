# Create a library for the folder
add_library(database
    accessor.h
    token.h
    token.c
)

# Include it
target_include_directories(database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})