cmake_minimum_required(VERSION 3.26.5)
project(order_matching_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_library(ome_lib STATIC src/ome.cpp)
target_include_directories(ome_lib PUBLIC ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ome_lib)

add_executable(benchmark benchmarking/benchmark.cpp)
target_link_libraries(benchmark PRIVATE ome_lib)