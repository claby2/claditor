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
  src/interface.cpp
  src/mode.cpp
  src/options.cpp
  src/parser.cpp
  src/position.cpp
  src/runtime.cpp)

if(ENABLE_TESTING)
  find_package(Catch2)
  if(Catch2_FOUND)
    add_executable(
      test
      tests/main.cpp
      tests/bind_count.cpp
      tests/buffer.cpp
      tests/color.cpp
      tests/colorscheme_manager.cpp
      tests/command.cpp
      tests/editor.cpp
      tests/history.cpp
      tests/mode.cpp
      tests/options.cpp
      tests/parser.cpp
      tests/position.cpp
      src/interface.cpp
      src/editor.cpp)
    target_compile_definitions(test PRIVATE UNIT_TEST)
    target_include_directories(test PUBLIC src/)
    target_link_libraries(test PUBLIC claditor Catch2::Catch2)
  endif()
endif()

find_package(Curses REQUIRED)
add_executable(clad src/main.cpp)
target_include_directories(clad PUBLIC ${CURSES_INCLUDE_DIR}
                                       libs/cxxopts/include)
target_link_libraries(clad PUBLIC claditor ${CURSES_LIBRARY})
