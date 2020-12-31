#include "editor.hpp"

#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <vector>

// Testing the editor:

// - The main goal of unit testing the editor is to test the functionality of
// the state machine.

// - Each test case targets a state and checks if it functions correctly.

// - Getting to a state in the finite state machine requires a certain
// sequence of inputs given as a string. This input string is converted to a
// vector of integers where it will be passed to the editor's interface and
// returned one by one every state enter call.

// - Number of lines and columns are defined in get_result

// - Escape (27) = \u001b

std::string get_result_with_dimensions(const std::string &buffer,
                                       const std::string &input,
                                       const int lines, const int columns) {
    std::string full_input = input + ":wq\n";
    std::vector<int> inputs(full_input.length());
    std::copy(full_input.begin(), full_input.end(), inputs.begin());
    std::stringstream file_stream(buffer);
    Editor editor("", file_stream);
    editor.set_interface(inputs, lines, columns);
    editor.start("");
    return editor.get_buffer_stream().str();
}

std::string get_result(const std::string &buffer, const std::string &input) {
    // Get result with default number of lines and columns
    const int LINES = 3;
    const int COLUMNS = 50;
    return get_result_with_dimensions(buffer, input, LINES, COLUMNS);
}

TEST_CASE("Editor normal first char", "[editor]") {
    std::string buffer = "hello world";
    std::string input = "3l0x";
    std::string expected = "ello world";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal first non blank char", "[editor]") {
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

TEST_CASE("Editor normal delete", "[editor]") {
    std::string buffer = "foo";
    std::string input = "2x";
    std::string expected = "o";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal end of file", "[editor]") {
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

TEST_CASE("Editor normal append after cursor", "[editor]") {
    std::string buffer = "foobar";
    std::string input = "3laz\u001b";
    std::string expected = "foobzar";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal append end of line", "[editor]") {
    std::string buffer = "fooba";
    std::string input = "Ar\u001b";
    std::string expected = "foobar";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal begin new line below", "[editor]") {
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

TEST_CASE("Editor normal begin new line above", "[editor]") {
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

TEST_CASE("Editor normal first line", "[editor]") {
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

TEST_CASE("Editor normal jump line", "[editor]") {
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

TEST_CASE("Editor normal center line", "[editor]") {
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

TEST_CASE("Editor normal delete line", "[editor]") {
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
    SECTION("Delete multiple lines") {
        std::string buffer =
            "line1\n"
            "line2\n"
            "line3\n"
            "line4\n"
            "line5";
        std::string input = "2j100dd";
        std::string expected =
            "line1\n"
            "line2";

        std::string result = get_result(buffer, input);
        REQUIRE(result == expected);
    }
}

TEST_CASE("Editor normal add count", "[editor]") {
    std::string buffer = "hello world foo bar";
    std::string input = "12lx";
    std::string expected = "hello world oo bar";

    std::string result = get_result(buffer, input);
    REQUIRE(result == expected);
}

TEST_CASE("Editor normal page down", "[editor]") {
    // When page down, the last line will become the first line
    // The number of lines in the buffer is the total number of lines subtracted
    // by the command height
    std::string buffer =
        "1\n"
        "2\n"
        "3\n"
        "4\n"
        "5";
    SECTION("Without bind count") {
        std::string input = std::string(1, 'f' & 0x1f) + "x";
        std::string expected =
            "1\n"
            "\n"
            "3\n"
            "4\n"
            "5";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("With bind count") {
        std::string input = "2" + std::string(1, 'f' & 0x1f) + "x";
        std::string expected =
            "1\n"
            "2\n"
            "\n"
            "4\n"
            "5";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor normal page up", "[editor]") {
    std::string buffer =
        "1\n"
        "2\n"
        "3\n"
        "4";
    SECTION("Without bind count") {
        std::string input = "G" + std::string(1, 'b' & 0x1f) + "x";
        std::string expected =
            "1\n"
            "2\n"
            "\n"
            "4";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("With bind count") {
        std::string input = "G2" + std::string(1, 'b' & 0x1f) + "x";
        std::string expected =
            "1\n"
            "\n"
            "3\n"
            "4";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor move up", "[editor]") {
    std::string buffer =
        "1\n"
        "2\n"
        "3\n"
        "4";
    SECTION("Without bind count") {
        std::string input = "Gkx";
        std::string expected =
            "1\n"
            "2\n"
            "\n"
            "4";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("With bind count") {
        std::string input = "G2kx";
        std::string expected =
            "1\n"
            "\n"
            "3\n"
            "4";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor move right", "[editor]") {
    std::string buffer = "hello world";
    SECTION("Without bind count") {
        std::string input = "lx";
        std::string expected = "hllo world";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("With bind count") {
        std::string input = "5lx";
        std::string expected = "helloworld";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor move down", "[editor]") {
    std::string buffer =
        "1\n"
        "2\n"
        "3\n"
        "4";
    SECTION("Without bind count") {
        std::string input = "jx";
        std::string expected =
            "1\n"
            "\n"
            "3\n"
            "4";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("With bind count") {
        std::string input = "2jx";
        std::string expected =
            "1\n"
            "2\n"
            "\n"
            "4";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("Move down more than buffer size") {
        // This section aims to ensure that "SIGSEGV - Segmentation violation
        // signal" is not produced
        std::string short_buffer =
            "1\n"
            "2";
        std::string input = "5jx";
        std::string expected = "1\n";
        const int lines = 50;
        const int columns = 50;

        std::string result =
            get_result_with_dimensions(short_buffer, input, lines, columns);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor move left", "[editor]") {
    std::string buffer = "hello world";
    SECTION("Without bind count") {
        std::string input = "10lhx";
        std::string expected = "hello word";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("With bind count") {
        std::string input = "10l5hx";
        std::string expected = "helloworld";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor insert backspace", "[editor]") {
    SECTION("Move line above") {
        std::string buffer =
            "hello\n"
            "world";
        std::string input = "ji\u007f\u001b";
        std::string expected = "helloworld";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("Delete single character") {
        std::string buffer = "foo";
        std::string input = "li\u007f\u001b";
        std::string expected = "oo";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor insert enter", "[editor]") {
    SECTION("Move substring down") {
        std::string buffer = "hello";
        std::string input = "li\u000a\u001b";
        std::string expected =
            "h\n"
            "ello";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("Insert new line") {
        std::string buffer = "";
        std::string input = "ihello\u000a\u001b";
        std::string expected = "hello\n";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor insert tab", "[editor]") {
    std::string buffer = "hello";
    SECTION("Tabs") {
        std::string input = ":set tabs\u000ai\u0009\u001b";
        std::string expected = "\thello";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("Spaces") {
        std::string input = ":set notabs | set tabsize=4\u000ai\u0009\u001b";
        std::string expected = "    hello";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor insert char", "[editor]") {
    std::string buffer = "bar";
    std::string input = "ifoo \u001b";
    std::string expected = "foo bar";

    std::string result = get_result(buffer, input);
    CHECK(result == expected);
}

TEST_CASE("Editor command backspace", "[editor]") {
    SECTION("Empty command line") {
        std::string buffer = "";
        std::string input = ":\u007fifoo\u001b";
        std::string expected = "foo";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("Filled command line") {
        std::string buffer =
            "1\n"
            "2\n"
            "3";
        std::string input = ":3\u007f2\u000ax";
        std::string expected =
            "1\n"
            "\n"
            "3";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}

TEST_CASE("Editor command enter", "[editor]") {
    std::string buffer =
        "1\n"
        "2\n"
        "3";
    std::string input = ":2\u000ax";
    std::string expected =
        "1\n"
        "\n"
        "3";

    std::string result = get_result(buffer, input);
    CHECK(result == expected);
}

TEST_CASE("Editor command char", "[editor]") {
    std::string buffer = "";
    std::string input = ":echo \"ifoo\"\u000a";
    std::string expected = "";

    std::string result = get_result(buffer, input);
    CHECK(result == expected);
}

TEST_CASE("Editor visual delete selection", "[editor]") {
    std::string buffer =
        "line_one\n"
        "line_two\n"
        "    line_three\n"
        "\n"
        "line_five";
    SECTION("Visual mode") {
        std::string input = "2lv2j3ld";
        std::string expected =
            "line_three\n"
            "\n"
            "line_five";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
    SECTION("Visual line mode") {
        std::string input = "V2jd";
        std::string expected =
            "\n"
            "line_five";

        std::string result = get_result(buffer, input);
        CHECK(result == expected);
    }
}
