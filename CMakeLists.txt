cmake_minimum_required(VERSION 3.17)
project(SharedCalculator)

set(CMAKE_CXX_STANDARD 17)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(shared_calculator)

# Enable testing
enable_testing()
add_subdirectory(tests)
