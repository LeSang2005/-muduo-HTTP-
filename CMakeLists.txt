cmake_minimum_required(VERSION 3.12)
project(http)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "DEbug flags" FORCE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE my CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(httpserver ${my})

target_compile_options(httpserver PRIVATE -fsanitize=leak -fno-omit-frame-pointer)
target_link_options(httpserver PRIVATE -fsanitize=leak)
target_link_libraries(httpserver muduo_net muduo_base pthread mysqlclient ssl crypto)