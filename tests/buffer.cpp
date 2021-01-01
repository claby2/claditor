#include "buffer.hpp"

#include <catch2/catch.hpp>
#include <string>

#include "position.hpp"

TEST_CASE("Buffer initial construction", "[buffer]") {
    // Buffer should be empty upon construction
    Buffer buffer;
    int size = buffer.get_size();
    Position expected_position = {0, 0};
    CHECK(size == 0);
    CHECK(buffer.position == expected_position);
}

TEST_CASE("Buffer get line length of empty line", "[buffer]") {
    Buffer buffer;
    buffer.lines = {""};
    int length = buffer.get_line_length(0);
    REQUIRE(length == 0);
}

TEST_CASE("Buffer get line length", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foobar"};
    int length = buffer.get_line_length(0);
    REQUIRE(length == 6);
}

TEST_CASE("Buffer get first non blank in empty line", "[buffer]") {
    Buffer buffer;
    buffer.lines = {""};
    int index = buffer.get_first_non_blank(0);
    REQUIRE(index == 0);
}

TEST_CASE("Buffer get first non blank in whitespace line", "[buffer]") {
    Buffer buffer;
    // If line only consists of whitespace characters the first non blank should
    // be equal to the length of the line
    buffer.lines = {" \t\r\n"};
    int index = buffer.get_first_non_blank(0);
    REQUIRE(index == 4);
}

TEST_CASE("Buffer get first non blank", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"    foo"};  // Four spaces + foo
    int index = buffer.get_first_non_blank(0);
    REQUIRE(index == 4);
}

TEST_CASE("Buffer set line", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foo"};
    buffer.set_line("bar", 0);
    std::string line = buffer.lines[0];
    REQUIRE(line == "bar");
}

TEST_CASE("Buffer push back line", "[buffer]") {
    Buffer buffer;
    buffer.push_back_line("foo");
    std::string line = buffer.lines[0];
    REQUIRE(line == "foo");
}

TEST_CASE("Buffer insert line", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foo", "bar"};
    buffer.insert_line("hello", 1);
    std::string line = buffer.lines[1];
    REQUIRE(line == "hello");
}

TEST_CASE("Buffer add string to line", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foo"};
    buffer.add_string_to_line("bar", 0);
    std::string line = buffer.lines[0];
    REQUIRE(line == "foobar");
}

TEST_CASE("Buffer erase", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foobar"};
    buffer.erase(3, 3, 0);  // Erase bar
    std::string line = buffer.lines[0];
    REQUIRE(line == "foo");
}

TEST_CASE("Buffer insert character", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foobar"};
    // File line 0 with ' ' 4 times from position 3
    buffer.insert_char(3, 4, ' ', 0);
    std::string line = buffer.lines[0];
    REQUIRE(line == "foo    bar");  // foo + 4 spaces + bar
}

TEST_CASE("Buffer remove line", "[buffer]") {
    Buffer buffer;
    buffer.lines = {"foo", "bar"};
    buffer.remove_line(0);
    std::string line = buffer.lines[0];
    REQUIRE(line == "bar");
}
