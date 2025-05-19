cmake_minimum_required(VERSION 3.10)

project(codec-amr VERSION 1.0.0 LANGUAGES C CXX)

file(GLOB SB_SRC "${PROJECT_SOURCE_DIR}/src/nb/*.c")
file(GLOB WB_SRC "${PROJECT_SOURCE_DIR}/src/wb/*.c")

# Create AMR-NB library
add_library(codec-amr ${SB_SRC} ${WB_SRC})
target_include_directories(codec-amr PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/nb
    ${PROJECT_SOURCE_DIR}/src/wb
)

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/src/main")


