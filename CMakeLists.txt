cmake_minimum_required(VERSION 3.0)

project(LOG)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SRC_DIR_LIST src/*.cc)

message(STATUS "SRC_DIR_LIST:${SRC_DIR_LIST}")

# find_path(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

message(STATUS "INCLUDE_PATH:${INCLUDE_PATH}")

include_directories(${INCLDUE_PATH})

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cc)