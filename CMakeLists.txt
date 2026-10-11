cmake_minimum_required(VERSION 3.26)
project(order_matching_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_library(ome_lib STATIC src/ome.cpp)
target_include_directories(ome_lib PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_options(ome_lib PRIVATE -Wall -Wextra)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ome_lib)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

add_executable(benchmark benchmarking/benchmark.cpp)
target_link_libraries(benchmark PRIVATE ome_lib)
target_compile_options(benchmark PRIVATE -Wall -Wextra)