# Unit tests using Google Test

# gRPC and Protobuf dependencies
find_package(PkgConfig REQUIRED)
find_package(Protobuf REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)
pkg_check_modules(PROTOBUF REQUIRED protobuf)

# Compile only test files from this directory
file(GLOB TEST_SOURCES "*t.cpp")

add_executable(calculator_tests
    ${TEST_SOURCES}
)

target_include_directories(calculator_tests
  PRIVATE
    ${CMAKE_BINARY_DIR}/shared_calculator/shared_calculator_leader
    ${CMAKE_SOURCE_DIR}/shared_calculator/shared_calculator_leader
    ${GRPC_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
)

target_link_libraries(calculator_tests
  PRIVATE
    gtest_main
    shared_calculator_leader_library
    ${GRPC_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
)

# Enable testing
enable_testing()
add_test(NAME CalculatorTests COMMAND calculator_tests)