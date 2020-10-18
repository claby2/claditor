#include <catch2/catch.hpp>

#include "position.hpp"

TEST_CASE("Position equal operator") {
    Position position1 = {1, 2};
    Position position2 = {1, 2};
    REQUIRE(position1 == position2);
}

TEST_CASE("Position no argument constructor") {
    Position position;
    Position expected_position = {0, 0};
    REQUIRE(position == expected_position);
}

TEST_CASE("Position argument constructor") {
    Position position(1, 2);
    Position expected_position = {1, 2};
    REQUIRE(position == expected_position);
}
