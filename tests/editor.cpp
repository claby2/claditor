#include "editor.hpp"

#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <vector>

std::string get_result(const std::string &buffer, const std::string &input) {
    std::string full_input = input + ":wq\n";
    std::vector<int> inputs(full_input.length());
    std::copy(full_input.begin(), full_input.end(), inputs.begin());
    std::stringstream file_stream(buffer);
    Editor editor("", file_stream);
    editor.set_interface_inputs(inputs);
    editor.start();
    return editor.get_buffer_stream().str();
}

TEST_CASE("Editor normal first char") {
    std::string buffer = "hello world";
    std::string input = "3l0x";
    std::string expected = "ello world";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal first non blank char") {
    std::string buffer =
        "    hello world\n"
        "foo bar";
    std::string input = "^xjx";
    std::string expected =
        "    ello world\n"
        "foo ar";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal delete") {
    std::string buffer = "foo";
    std::string input = "x";
    std::string expected = "oo";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal end of file") {
    std::string buffer =
        "line1\n"
        "line2\n"
        "    line3";
    std::string input = "Gx";
    std::string expected =
        "line1\n"
        "line2\n"
        "    ine3";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal append after cursor") {
    std::string buffer = "foobar";
    std::string input = "3laz\u001b";
    std::string expected = "foobzar";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal append end of line") {
    std::string buffer = "fooba";
    std::string input = "Ar\u001b";
    std::string expected = "foobar";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal begin new line below") {
    SECTION("From middle of file") {
        std::string buffer =
            "line1\n"
            "line3";
        std::string input = "oline2\u001b";
        std::string expected =
            "line1\n"
            "line2\n"
            "line3";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("From end of file") {
        // Newline should be created
        std::string buffer = "foo";
        std::string input = "obar\u001b";
        std::string expected =
            "foo\n"
            "bar";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor normal begin new line above") {
    SECTION("From middle of file") {
        std::string buffer =
            "line1\n"
            "line3";
        std::string input = "jOline2\u001b";
        std::string expected =
            "line1\n"
            "line2\n"
            "line3";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("From start of file") {
        // Newline should be created
        std::string buffer = "bar";
        std::string input = "Ofoo\u001b";
        std::string expected =
            "foo\n"
            "bar";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor normal first line") {
    std::string buffer =
        "    line1\n"
        "line2\n"
        "line3\n"
        "line4\n"
        "line5";
    std::string input = "4jxggx";
    std::string expected =
        "    ine1\n"
        "line2\n"
        "line3\n"
        "line4\n"
        "ine5";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal jump line") {
    std::string buffer =
        "line1\n"
        "line2\n"
        "line3\n"
        "line4";
    std::string input = "3ggx";
    std::string expected =
        "line1\n"
        "line2\n"
        "ine3\n"
        "line4";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal center line") {
    // If bind count is not empty, zz should act like gg
    // It also centers the screen, however, this behavior cannot be tested
    std::string buffer =
        "line1\n"
        "line2\n"
        "line3\n"
        "line4";
    std::string input = "3zzx";
    std::string expected =
        "line1\n"
        "line2\n"
        "ine3\n"
        "line4";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal delete line") {
    SECTION("No throw when file has no lines") {
        std::string buffer = "";
        std::string input = "dddd";
        REQUIRE_NOTHROW(get_result(buffer, input));
    }
    SECTION("Regular delete") {
        std::string buffer =
            "line1\n"
            "line2";
        std::string input = "dd";
        std::string expected = "line2";

        std::string result = get_result(buffer, input);
        REQUIRE(result == expected);
    }
}
