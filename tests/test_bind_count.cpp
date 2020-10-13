#include <catch2/catch.hpp>

#include "bind_count.hpp"

TEST_CASE("Bind Count initial construction") {
    BindCount bind_count;
    bool empty = bind_count.empty();
    REQUIRE(empty);
}

TEST_CASE("Bind Count get value when empty") {
    BindCount bind_count;
    // Value returned should be 1 when bind count is empty
    int value = bind_count.get_value();
    REQUIRE(value == 1);
}

TEST_CASE("Bind Count single digit") {
    BindCount bind_count;
    bind_count.add_digit(5);
    int value = bind_count.get_value();
    REQUIRE(value == 5);
}

TEST_CASE("Bind Count add multiple digits") {
    BindCount bind_count;
    bind_count.add_digit(1);
    bind_count.add_digit(0);
    int value = bind_count.get_value();
    REQUIRE(value == 10);
}
