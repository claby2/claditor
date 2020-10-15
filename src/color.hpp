#ifndef CLADITOR_COLOR_HPP
#define CLADITOR_COLOR_HPP

#include <string>

extern const int COLORS_DEFINED;
extern const int PAIRS_DEFINED;

enum class ColorType : short {
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

    short r;
    short g;
    short b;
};

struct Colorscheme {
    Colorscheme();
    explicit Colorscheme(const std::string&);

    Color background;
    Color foreground;
    Color comment;
    Color accent;
    Color color1;
    Color color2;
    Color color3;
    Color color4;
    Color color5;
    Color color6;
};

struct ColorPair {
    ColorType foreground;
    ColorBackground background;
};

short get_color_pair_index(ColorType, ColorBackground);
#endif
