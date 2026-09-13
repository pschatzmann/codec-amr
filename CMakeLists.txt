cmake_minimum_required(VERSION 3.10)

if (DEFINED ESP_PLATFORM)
  # idf component
  idf_component_register(
      SRC_DIRS src/nb src/wb
      INCLUDE_DIRS src src/nb src/wb
  )

  target_compile_options(${COMPONENT_LIB} PUBLIC -Wno-error)

else()

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
## we use default IF1: set alternatives formats with -DETSI or -DIF2
target_compile_options(codec-amr PUBLIC "-Wstack-usage=2800" )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/main")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples/AMRCodecExample")

endif()


