cmake_minimum_required(VERSION 3.17)

set(COMMON_LIBRARY "shared_calculator_common")

file(GLOB_RECURSE ALL_SOURCES SOURCES
    "*.h"
    "*.cpp"
)

add_library(${COMMON_LIBRARY} ${ALL_SOURCES})

target_include_directories(${COMMON_LIBRARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
