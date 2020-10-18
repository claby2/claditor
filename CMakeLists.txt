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
  src/colorscheme_manager.cpp
  src/command.cpp
  src/editor.cpp
  src/file.cpp
  src/history.cpp
  src/mode.cpp
  src/parser.cpp
  src/position.cpp
  src/runtime.cpp)

if(ENABLE_TESTING)
  find_package(Catch2)
  if(Catch2_FOUND)
    add_executable(
      test
      tests/test_main.cpp
      tests/test_bind_count.cpp
      tests/test_buffer.cpp
      tests/test_color.cpp
      tests/test_colorscheme_manager.cpp
      tests/test_command.cpp
      tests/test_editor.cpp
      tests/test_file.cpp
      tests/test_history.cpp
      tests/test_mode.cpp
      tests/test_parser.cpp
      tests/test_position.cpp
      src/interface.cpp)
    target_compile_definitions(test PRIVATE UNIT_TEST)
    target_include_directories(test PUBLIC src/)
    target_link_libraries(test PUBLIC claditor Catch2::Catch2)
  endif()
endif()

find_package(Curses REQUIRED)
add_executable(clad src/main.cpp src/interface.cpp)
target_include_directories(clad PUBLIC ${CURSES_INCLUDE_DIR})
target_link_libraries(clad PUBLIC claditor ${CURSES_LIBRARY})
