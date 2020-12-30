#include "options.hpp"

#include <catch2/catch.hpp>
#include <string>

TEST_CASE("Options get valid int option") {
    Options options;
    REQUIRE_NOTHROW(options.get_int_option("tabsize"));
}

TEST_CASE("Options get valid bool option") {
    Options options;
    REQUIRE_NOTHROW(options.get_bool_option("number"));
}

TEST_CASE("Options get valid string option") {
    Options options;
    REQUIRE_NOTHROW(options.get_string_option("colorscheme"));
}

TEST_CASE("Options get option with invalid option") {
    Options options;
    CHECK_THROWS(options.get_int_option("invalid_option"));
    CHECK_THROWS(options.get_bool_option("invalid_option"));
    CHECK_THROWS(options.get_string_option("invalid_option"));
}

TEST_CASE("Options set valid int option") {
    Options options;
    options.set_option("tabsize=2");
    int value = options.get_int_option("tabsize");
    int expected_value = 2;
    REQUIRE(value == expected_value);
}

TEST_CASE("Options set invalid int option with no number") {
    Options options;
    bool valid_option = options.set_option("tabsize=");
    REQUIRE_FALSE(valid_option);
}

TEST_CASE("Options set invalid int option with bad number argument") {
    Options options;
    bool valid_option = options.set_option("tabsize=123invalid");
    REQUIRE_FALSE(valid_option);
}

TEST_CASE("Options set invalid int option with no equal sign") {
    Options options;
    bool valid_option = options.set_option("tabsize");
    REQUIRE_FALSE(valid_option);
}

TEST_CASE("Options set valid positive bool option") {
    Options options;
    // Verify that tabs default value is false
    bool initial_value = options.get_bool_option("tabs");
    CHECK_FALSE(initial_value);

    options.set_option("tabs");
    bool expected = true;
    bool value = options.get_bool_option("tabs");
    REQUIRE(value == expected);
}

TEST_CASE("Options set valid negative bool option") {
    Options options;
    // Verify that number default value is true
    bool initial_value = options.get_bool_option("number");
    CHECK(initial_value);

    options.set_option("nonumber");
    bool expected = false;
    bool value = options.get_bool_option("number");
    REQUIRE(value == expected);
}

TEST_CASE("Options set invalid negative bool option") {
    Options options;
    int valid_option = options.set_option("noinvalid_option");
    REQUIRE_FALSE(valid_option);
}

TEST_CASE("Options set valid string option") {
    Options options;
    // Verify that colorscheme initial value is empty string
    std::string initial_value = options.get_string_option("colorscheme");
    CHECK(initial_value.empty());

    options.set_option("colorscheme=foo");
    std::string expected = "foo";
    std::string value = options.get_string_option("colorscheme");
    REQUIRE(value == expected);
}
