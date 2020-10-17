#include <catch2/catch.hpp>

#include "mode.hpp"

TEST_CASE("Mode get type") {
    Mode mode(ModeType::VISUAL);
    ModeType type = mode.get_type();
    REQUIRE(type == ModeType::VISUAL);
}

TEST_CASE("Mode set type") {
    Mode mode(ModeType::NORMAL);
    mode.set_type(ModeType::VISUAL);
    ModeType type = mode.get_type();
    REQUIRE(type == ModeType::VISUAL);
}
