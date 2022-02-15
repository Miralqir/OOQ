cmake_minimum_required(VERSION 3.13)

project(OOQ VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	# Disable Microsoft extensions
	add_compile_options(/Za)
endif()

configure_file(config.h.in config.h @ONLY)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)

set(SRC
	src/main.cpp
	src/utilities.cpp
	src/manager.cpp
	src/render.cpp
	src/input.cpp
	src/game.cpp
	src/ui.cpp
)

add_executable(OOQ WIN32 ${SRC})
target_link_libraries(OOQ SDL2::Main SDL2::Image SDL2::TTF)
