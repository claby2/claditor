#include <catch2/catch.hpp>
#include <string>

#include "parser.hpp"

TEST_CASE("Parser") {
    Parser parser("tests/test_files/parser_test.clad");
    CHECK(parser["1property"] == "1value");
    CHECK(parser["2property"] == "2value");
    CHECK(parser["3property"] == "second");
    std::string commented = parser["commented"];
    // Commented keys and values should be ignored
    CHECK(commented.empty());
}
