cmake_minimum_required(VERSION 3.19)

add_library(SPIFBlockDevice STATIC SPIFBlockDevice.cpp SPIFBlockDevice.h)

target_include_directories(SPIFBlockDevice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SPIFBlockDevice mbed-core-flags)