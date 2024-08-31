# Create a library for the folder
add_library(networking
    networking.c
    networking.h
    schema.h
    comms.h
    utils.h
    utils.c
    certificates.h
    certificates.c
    web-utils.h
    web-utils.c
)

# Include it
target_include_directories(networking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(networking PRIVATE -g -Wall)