cmake_minimum_required(VERSION 3.21)
project(GBE)

set(CMAKE_CXX_STANDARD 14)

set(SDL2_INCLUDE_DIR sdl/include)
set(SDL2_LIB_DIR sdl/lib/x86)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(GBE src/main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2)
