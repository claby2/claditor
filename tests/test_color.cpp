#include <catch2/catch.hpp>

#include "color.hpp"

bool colors_equal(const Color& color1, const Color& color2) {
    return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b;
}

bool is_default_colorscheme(const Colorscheme& colorscheme) {
    Color BLACK = {0, 0, 0};
    Color WHITE = {255, 255, 255};
    return colors_equal(colorscheme.background, BLACK) &&
           colors_equal(colorscheme.foreground, WHITE) &&
           colors_equal(colorscheme.comment, WHITE) &&
           colors_equal(colorscheme.accent, WHITE) &&
           colors_equal(colorscheme.color1, WHITE) &&
           colors_equal(colorscheme.color2, WHITE) &&
           colors_equal(colorscheme.color3, WHITE) &&
           colors_equal(colorscheme.color4, WHITE) &&
           colors_equal(colorscheme.color5, WHITE) &&
           colors_equal(colorscheme.color6, WHITE);
}

TEST_CASE("Color empty constructor") {
    Color color;
    REQUIRE(colors_equal(color, {0, 0, 0}));
}

TEST_CASE("Color constructor") {
    Color color(1, 2, 3);
    REQUIRE(colors_equal(color, {1, 2, 3}));
}

TEST_CASE("Colorscheme default colorscheme") {
    Colorscheme colorscheme;
    // Test default color scheme
    bool is_default = is_default_colorscheme(colorscheme);
    REQUIRE(is_default);
}

TEST_CASE("Colorscheme parse file without valid properties") {
    // Read file that doesn't specify any valid color properties
    Colorscheme colorscheme("tests/test_files/color_invalid.clad");
    // If no valid properties are given, use default colorscheme
    bool is_default = is_default_colorscheme(colorscheme);
    REQUIRE(is_default);
}

TEST_CASE("Colorscheme parse file") {
    Colorscheme colorscheme("tests/test_files/color_valid.clad");
    CHECK(colors_equal(colorscheme.background, {0, 0, 0}));
    CHECK(colors_equal(colorscheme.foreground, {0, 0, 1000}));
    CHECK(colors_equal(colorscheme.comment, {0, 1000, 0}));
    CHECK(colors_equal(colorscheme.accent, {0, 1000, 1000}));
    CHECK(colors_equal(colorscheme.color1, {1000, 0, 0}));
    CHECK(colors_equal(colorscheme.color2, {1000, 0, 1000}));
    CHECK(colors_equal(colorscheme.color3, {1000, 1000, 0}));
    CHECK(colors_equal(colorscheme.color4, {1000, 1000, 1000}));
    CHECK(colors_equal(colorscheme.color5, {0, 0, 0}));
    CHECK(colors_equal(colorscheme.color6, {0, 0, 0}));
}
