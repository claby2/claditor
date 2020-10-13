cmake_minimum_required(VERSION 3.10)

project(claditor CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TESTING "Enable Test Builds" ON)

add_library(
  claditor
  src/bind_count.cpp
  src/buffer.cpp
  src/color.cpp
  src/editor.cpp
  src/history.cpp
  src/parser.cpp
  src/runtime.cpp)

if(ENABLE_TESTING)
  find_package(Catch2)
  if(Catch2_FOUND)
    add_executable(
      test tests/test_main.cpp tests/test_bind_count.cpp tests/test_buffer.cpp
           tests/test_color.cpp tests/test_history.cpp tests/test_parser.cpp)
    target_include_directories(test PUBLIC src/)
    target_link_libraries(test claditor Catch2::Catch2)
  endif()
endif()

find_package(Curses REQUIRED)
add_executable(clad src/main.cpp)
target_include_directories(clad PUBLIC ${CURSES_INCLUDE_DIR})
target_link_libraries(clad claditor ${CURSES_LIBRARY})
