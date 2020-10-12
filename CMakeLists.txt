project(claditor CXX)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(claditor src/main.cpp src/bind_count.cpp src/buffer.cpp
                        src/editor.cpp src/history.cpp)
target_link_libraries(claditor ${CURSES_LIBRARY})
