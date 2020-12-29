#include "colorscheme.hpp"

#include <catch2/catch.hpp>

bool is_default_colorscheme(const Colorscheme &colorscheme) {
    return Color::black() == colorscheme.background &&
           Color::white() == colorscheme.foreground &&
           Color::white() == colorscheme.comment &&
           Color::white() == colorscheme.accent &&
           Color::white() == colorscheme.color1 &&
           Color::white() == colorscheme.color2 &&
           Color::white() == colorscheme.color3 &&
           Color::white() == colorscheme.color4 &&
           Color::white() == colorscheme.color5 &&
           Color::white() == colorscheme.color6;
}

TEST_CASE("Colorscheme default colorscheme") {
    Colorscheme colorscheme;
    // Test default color scheme
    bool is_default = is_default_colorscheme(colorscheme);
    REQUIRE(is_default);
}

TEST_CASE("Colorscheme parse file without valid properties") {
    // Read file that doesn't specify any valid color properties
    std::string file_string =
        "// Invalid colorscheme\n"
        "\n"
        "// Background has invalid color\n"
        "background = #ghijkl\n"
        "invalid = #ffffff\n"
        "\n"
        "// foreground, comment, accent, color1, color2, color3, color4, \n"
        "color5, and\n"
        "// color 6 are not defined";
    std::stringstream file_stream(file_string);
    Colorscheme colorscheme(file_stream);
    // If no valid properties are given, use default colorscheme
    bool is_default = is_default_colorscheme(colorscheme);
    REQUIRE(is_default);
}

TEST_CASE("Colorscheme parse file") {
    std::string file_string =
        "// Valid colorscheme\n"
        "\n"
        "background=#000000\n"
        "foreground= #0000FF\n"
        "comment =#00FF00\n"
        "accent = #00FFFF\n"
        "color1 = #FF0000\n"
        "color2 = #FF00FF\n"
        "color3 = #FFff00\n"
        "color4 = #ffffff\n"
        "color5 = #000000\n"
        "color6 = #000000\n";
    std::vector<Color> expected = {
        {0, 0, 0},    {0, 0, 1000},    {0, 1000, 0},    {0, 1000, 1000},
        {1000, 0, 0}, {1000, 0, 1000}, {1000, 1000, 0}, {1000, 1000, 1000},
        {0, 0, 0},    {0, 0, 0}};

    std::stringstream file_stream(file_string);
    Colorscheme colorscheme(file_stream);
    bool is_default = is_default_colorscheme(colorscheme);
    std::vector<Color> result = {colorscheme.background, colorscheme.foreground,
                                 colorscheme.comment,    colorscheme.accent,
                                 colorscheme.color1,     colorscheme.color2,
                                 colorscheme.color3,     colorscheme.color4,
                                 colorscheme.color5,     colorscheme.color6};

    REQUIRE_FALSE(is_default);
    REQUIRE(expected == result);
}
