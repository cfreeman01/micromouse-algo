cmake_minimum_required(VERSION 3.2)
project(micrmouse-algo)

file(GLOB_RECURSE SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(micromouse-algo
            ${SRC})

include_directories(micromouse-algo 
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware-mock)