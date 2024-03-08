# Create a library for the folder
add_library(logic
    actions.c
    actions.h
    state.h
)

# Include it
target_include_directories(logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
