#include "color.hpp"

#include <utility>

#include "parser.hpp"

const int COLORS_DEFINED = 10;
const int PAIRS_DEFINED = 18;

bool is_valid_hex(std::string hex) {
    const int EXPECTED_LENGTH = 7;  // Expected length of the entire string
    return hex.length() == EXPECTED_LENGTH && hex[0] == '#' &&
           (hex.substr(1, EXPECTED_LENGTH - 1))
                   .find_first_not_of("0123456789ABCDEFabcdef") ==
               std::string::npos;
}

short get_scaled_value(const std::string &hex, int n) {
    // Convert RGB values from hex string to zero to 0 - 1000 scale for ncurses
    const int SCALED_MAX_VALUE = 1000;
    const int MAX_VALUE = 255;
    const int BASE = 16;
    std::string substring = hex.substr((n * 2) + 1, 2);
    return static_cast<short>(
        (std::stoi(substring, nullptr, BASE) * SCALED_MAX_VALUE) / MAX_VALUE);
}

Color get_black() { return {0, 0, 0}; }

Color get_white() { return {1000, 1000, 1000}; }

Color get_color_from_hex(const std::string &hex) {
    Color color;
    color.r = get_scaled_value(hex, 0);
    color.g = get_scaled_value(hex, 1);
    color.b = get_scaled_value(hex, 2);
    return color;
}

Color get_color(const std::string &hex, Color fallback_color) {
    return is_valid_hex(hex) ? get_color_from_hex(hex) : fallback_color;
}

Color::Color() : r(0), g(0), b(0) {}

Color::Color(short new_r, short new_g, short new_b)
    : r(new_r), g(new_g), b(new_b) {}

Colorscheme::Colorscheme()
    : background(get_black()),
      foreground(get_white()),
      comment(get_white()),
      accent(get_white()),
      color1(get_white()),
      color2(get_white()),
      color3(get_white()),
      color4(get_white()),
      color5(get_white()),
      color6(get_white()) {}

Colorscheme::Colorscheme(const std::string &file_path) {
    // Set colorscheme from path
    Parser parser(file_path);
    // Set colors of colorscheme
    background = get_color(parser["background"], get_black());
    foreground = get_color(parser["foreground"], get_white());
    comment = get_color(parser["comment"], get_white());
    accent = get_color(parser["accent"], get_white());
    color1 = get_color(parser["color1"], get_white());
    color2 = get_color(parser["color2"], get_white());
    color3 = get_color(parser["color3"], get_white());
    color4 = get_color(parser["color4"], get_white());
    color5 = get_color(parser["color5"], get_white());
    color6 = get_color(parser["color6"], get_white());
}

short get_color_pair_index(ColorForeground foreground,
                           ColorBackground background) {
    return static_cast<short>(
        (background == ColorBackground::ACCENT ? COLORS_DEFINED - 1 : 0) +
        static_cast<short>(foreground));
}
