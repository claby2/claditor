#include "color.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Color is valid hex color pass valid", "[color]") {
    std::string hex_color = "#0fEdb3";
    int is_valid = is_valid_hex_color(hex_color);
    REQUIRE(is_valid);
}

TEST_CASE("Color is valid hex color pass bad length", "[color]") {
    std::string hex_color = "#1234567";
    int is_valid = is_valid_hex_color(hex_color);
    CHECK_FALSE(is_valid);
}

TEST_CASE("Color is valid hex color pass no hash prefix", "[color]") {
    std::string hex_color = "!123456";
    int is_valid = is_valid_hex_color(hex_color);
    CHECK_FALSE(is_valid);
}
// abcdefghijklmnopqrstuvwxyz
TEST_CASE("Color is valid hex color pass bad characters") {
    std::string hex_color = "#z12345";
    int is_valid = is_valid_hex_color(hex_color);
    CHECK_FALSE(is_valid);
}

TEST_CASE("Color get scaled value", "[color]") {
    std::string hex_color = "#FF00ff";
    short expected_1 = 1000;
    short result_1 = get_scaled_value(hex_color, 0);
    short expected_2 = 0;
    short result_2 = get_scaled_value(hex_color, 1);
    short expected_3 = 1000;
    short result_3 = get_scaled_value(hex_color, 2);
    CHECK(result_1 == expected_1);
    CHECK(result_2 == expected_2);
    CHECK(result_3 == expected_3);
}

TEST_CASE("Color empty constructor", "[color]") {
    Color color;
    Color expected = {0, 0, 0};
    REQUIRE(color == expected);
}

TEST_CASE("Color constructor", "[color]") {
    Color color(1, 2, 3);
    Color expected = {1, 2, 3};
    REQUIRE(color == expected);
}

TEST_CASE("Color get accent background color pair index", "[color]") {
    ColorForeground foreground = ColorForeground::DEFAULT;
    ColorBackground background = ColorBackground::ACCENT;
    short index = get_color_pair_index(foreground, background);
    short expected = 10;
    // Color pair 10
    REQUIRE(index == expected);
}

TEST_CASE("Color get default background color pair index", "[color]") {
    ColorForeground foreground = ColorForeground::DEFAULT;
    ColorBackground background = ColorBackground::DEFAULT;
    short index = get_color_pair_index(foreground, background);
    short expected = 1;
    // Color pair 1
    REQUIRE(index == expected);
}
