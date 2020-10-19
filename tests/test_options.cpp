#include <catch2/catch.hpp>

#include "options.hpp"

TEST_CASE("Options operator valid int option") {
    Options options;
    REQUIRE_NOTHROW(options["tabsize"]);
}

TEST_CASE("Options operator valid bool option") {
    Options options;
    REQUIRE_NOTHROW(options["number"]);
}

TEST_CASE("Options operator invalid option") {
    Options options;
    REQUIRE_THROWS(options["invalid_option"]);
}

TEST_CASE("Options set valid int option") {
    Options options;
    options.set_option("tabsize=2");
    int value = options["tabsize"];
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
    bool valid_option = options.set_option("number");
    REQUIRE(valid_option);
}

TEST_CASE("Options set valid negative bool option") {
    Options options;
    options.set_option("nonumber");
    int value = options["number"];
    int expected_value = 0;
    REQUIRE(value == expected_value);
}

TEST_CASE("Options set invalid negative bool option") {
    Options options;
    int valid_option = options.set_option("noinvalid_option");
    REQUIRE_FALSE(valid_option);
}
