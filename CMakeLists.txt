cmake_minimum_required(VERSION 3.7)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPackageHandleStandardArgs)
include(FindSDL2)

file (GLOB SRC_FILES src/*.cpp src/zlib/*.c)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(png_decoder ${SRC_FILES})
target_link_libraries(png_decoder SDL2::SDL2main SDL2::SDL2)