#ifndef CLADITOR_COLOR_HPP
#define CLADITOR_COLOR_HPP

#include <string>

extern const int COLORS_DEFINED;
extern const int PAIRS_DEFINED;

bool is_valid_hex_color(const std::string &);

short get_scaled_value(const std::string &, int);

enum class ColorForeground : short {
    DEFAULT = 1,
    COMMENT = 2,
    ACCENT = 3,
    COLOR1 = 4,
    COLOR2 = 5,
    COLOR3 = 6,
    COLOR4 = 7,
    COLOR5 = 8,
    COLOR6 = 9
};

enum class ColorBackground { DEFAULT, ACCENT };

struct Color {
    Color();
    Color(short, short, short);

    static Color black();
    static Color white();

    bool operator==(const Color &) const;

    short r;
    short g;
    short b;
};

Color get_color(const std::string &, Color);

short get_color_pair_index(ColorForeground, ColorBackground);
#endif
