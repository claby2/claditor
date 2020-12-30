#include "mode.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Mode get type", "[mode]") {
    Mode mode(ModeType::VISUAL);
    ModeType type = mode.get_type();
    REQUIRE(type == ModeType::VISUAL);
}

TEST_CASE("Mode set type", "[mode]") {
    Mode mode(ModeType::NORMAL);
    mode.set_type(ModeType::VISUAL);
    ModeType type = mode.get_type();
    REQUIRE(type == ModeType::VISUAL);
}
