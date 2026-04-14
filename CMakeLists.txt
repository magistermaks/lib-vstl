cmake_minimum_required(VERSION 3.20)

project(VSTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(vstl
		vstl.cpp
		vstl.hpp
)

target_include_directories(vstl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(vstl_demo
		demo.cpp
		demo-module.cpp
)

target_link_libraries(vstl_demo PRIVATE vstl)