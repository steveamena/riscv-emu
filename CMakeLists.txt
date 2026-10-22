cmake_minimum_required(VERSION 3.20)

project(riscv-emu)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin)

# Main App
file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(riscv-emu ${SRC_FILES})

target_include_directories(riscv-emu PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
