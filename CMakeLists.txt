cmake_minimum_required(VERSION 3.20)

project(riscv-emu)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin)


# Main App
add_executable(riscv-emu
                src/cpu.cpp
                src/main.cpp
                src/memory.cpp)

target_include_directories(riscv-emu PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Test configuration
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/063de7e9578f82b369302001269680b4b1553359.zip
)

FetchContent_MakeAvailable(googletest)

# Test build
enable_testing()

add_executable(
                test_riscv_emu
                tests/test_cpu.cc
                tests/test_memory.cc
                src/cpu.cpp
                src/memory.cpp
)
target_link_libraries(
  test_riscv_emu
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_riscv_emu)