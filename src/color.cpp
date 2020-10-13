#include "color.hpp"
#include "parser.hpp"

// Define color constants for fallback
const Color BLACK = {0, 0, 0};
const Color WHITE = {255, 255, 255};

bool is_valid_hex(std::string hex) {
    return hex.length() == 7 && hex[0] == '#' &&
           (hex.substr(1, 6)).find_first_not_of("0123456789ABCDEFabcdef") ==
               std::string::npos;
}

Color get_color_from_hex(std::string hex) {
    Color color;
    // Convert rgb values from hex string to zero to 0 - 1000 scale for ncurses
    color.r = static_cast<short>(
        (std::stoi(hex.substr(1, 2), nullptr, 16) * 1000) / 255);
    color.g = static_cast<short>(
        (std::stoi(hex.substr(3, 2), nullptr, 16) * 1000) / 255);
    color.b = static_cast<short>(
        (std::stoi(hex.substr(5, 2), nullptr, 16) * 1000) / 255);
    return color;
}

Color get_color(const std::string &hex, Color fallback_color) {
    return is_valid_hex(hex) ? get_color_from_hex(hex) : fallback_color;
}

Color::Color() : r(0), g(0), b(0) {}

Color::Color(short new_r, short new_g, short new_b)
    : r(new_r), g(new_g), b(new_b) {}

Colorscheme::Colorscheme()
    : background(BLACK),
      foreground(WHITE),
      comment(WHITE),
      accent(WHITE),
      color1(WHITE),
      color2(WHITE),
      color3(WHITE),
      color4(WHITE),
      color5(WHITE),
      color6(WHITE) {}

Colorscheme::Colorscheme(std::string file_path) {
    // Set colorscheme from path
    Parser parser(file_path);
    // Set colors of colorscheme
    background = get_color(parser["background"], BLACK);
    foreground = get_color(parser["foreground"], WHITE);
    comment = get_color(parser["comment"], WHITE);
    accent = get_color(parser["accent"], WHITE);
    color1 = get_color(parser["color1"], WHITE);
    color2 = get_color(parser["color2"], WHITE);
    color3 = get_color(parser["color3"], WHITE);
    color4 = get_color(parser["color4"], WHITE);
    color5 = get_color(parser["color5"], WHITE);
    color6 = get_color(parser["color6"], WHITE);
}
