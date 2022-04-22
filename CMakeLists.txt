cmake_minimum_required(VERSION 3.21)
project(GBE)

set(CMAKE_CXX_STANDARD 14)

set(SDL2_INCLUDE_DIR sdl/include)
set(SDL2_LIB_DIR sdl/lib/x86)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(GBE src/main.cpp src/emu.cpp src/emu.h src/cart.cpp src/cart.h src/cpu.cpp src/cpu.h src/inst.cpp src/inst.h src/ram.cpp src/ram.h src/stack.cpp src/stack.h src/screen.cpp src/screen.h src/io.cpp src/io.h src/timer.cpp src/timer.h src/ppu.cpp src/ppu.h)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2)
