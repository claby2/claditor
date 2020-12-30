#include "parser.hpp"

#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

TEST_CASE("Parser color file") {
    std::string file_string =
        "// Comments should be ignored\n"
        "// commented = comment\n"
        "\n"
        "1property=1value // Comment commented = comment //\n"
        "\n"
        "2property   =   2value\n"
        "\n"
        "// The value of '3property' should be 'second'\n"
        "3property = first\n"
        "3property = second\n";
    std::stringstream file_stream(file_string);
    Parser parser(FileType::COLOR, file_stream);
    std::unordered_map<std::string, std::string> expected = {
        {"1property", "1value"},
        {"2property", "2value"},
        {"3property", "second"},
    };

    std::unordered_map<std::string, std::string> result =
        parser.get_color_content();
    CHECK(result == expected);

    // Commented keys and values should be ignored
    std::string commented = result["commented"];
    CHECK(commented.empty());
}

TEST_CASE("Parser config file") {
    std::string file_string =
        "// Config test\n"
        "\n"
        "set colorscheme=default // Comments should be ignored\n"
        "colo \t\n"
        "\n"
        "w    \n";
    std::stringstream file_stream(file_string);
    Parser parser(FileType::CONFIG, file_stream);
    std::vector<std::string> expected = {
        "set colorscheme=default",
        "colo",
        "w",
    };

    std::vector<std::string> result = parser.get_config_content();
    REQUIRE(result == expected);
}
