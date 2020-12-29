#include "color.hpp"

#include <utility>

#include "parser.hpp"

const int COLORS_DEFINED = 10;
const int PAIRS_DEFINED = 18;

bool is_valid_hex_color(const std::string &hex_color) {
    const int EXPECTED_LENGTH = 7;  // Expected length of the entire string
    return hex_color.length() == EXPECTED_LENGTH && hex_color[0] == '#' &&
           (hex_color.substr(1, EXPECTED_LENGTH - 1))
                   .find_first_not_of("0123456789ABCDEFabcdef") ==
               std::string::npos;
}

short get_scaled_value(const std::string &hex_color, int n) {
    // Convert RGB values from hex string to zero to 0 - 1000 scale for ncurses
    const int SCALED_MAX_VALUE = 1000;
    const int MAX_VALUE = 255;
    const int BASE = 16;
    std::string substring = hex_color.substr((n * 2) + 1, 2);
    return static_cast<short>(
        (std::stoi(substring, nullptr, BASE) * SCALED_MAX_VALUE) / MAX_VALUE);
}

Color::Color() : r(0), g(0), b(0) {}

Color::Color(short r, short g, short b) : r(r), g(g), b(b) {}

Color Color::black() { return {0, 0, 0}; }

Color Color::white() { return {1000, 1000, 1000}; }

bool Color::operator==(const Color &color) const {
    return r == color.r && g == color.g && b == color.b;
}

Color get_color(const std::string &hex_color, Color fallback_color) {
    // Get color object from hex
    // fallback to fallback_color if given string is not valid hex color
    if (is_valid_hex_color(hex_color)) {
        return {get_scaled_value(hex_color, 0), get_scaled_value(hex_color, 1),
                get_scaled_value(hex_color, 2)};
    }
    return fallback_color;
}

short get_color_pair_index(ColorForeground foreground,
                           ColorBackground background) {
    return static_cast<short>(
        (background == ColorBackground::ACCENT ? COLORS_DEFINED - 1 : 0) +
        static_cast<short>(foreground));
}
