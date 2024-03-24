# Create a library for the folder
add_library(networking
    networking.c
    networking.h
    schema.h
    comms.h
)

# Include it
target_include_directories(networking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
