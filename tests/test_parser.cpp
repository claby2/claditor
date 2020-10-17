#include <catch2/catch.hpp>
#include <string>
#include <vector>

#include "parser.hpp"

TEST_CASE("Parser color file") {
    Parser parser(FileType::COLOR, "tests/test_files/parser_color_test.clad");
    CHECK(parser["1property"] == "1value");
    CHECK(parser["2property"] == "2value");
    CHECK(parser["3property"] == "second");
    std::string commented = parser["commented"];
    // Commented keys and values should be ignored
    CHECK(commented.empty());
}

TEST_CASE("Parser config file") {
    Parser parser(FileType::CONFIG, "tests/test_files/parser_config_test.clad");
    std::vector<std::string> config_content = parser.get_config_content();
    std::vector<std::string> expected_config_content{"colorscheme default",
                                                     "colo", "w"};
    REQUIRE(config_content == expected_config_content);
}
