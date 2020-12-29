#ifndef CLADITOR_COLORSCHEME_HPP
#define CLADITOR_COLORSCHEME_HPP

#include <sstream>

#include "color.hpp"

struct Colorscheme {
    Colorscheme();
    explicit Colorscheme(const std::stringstream &);

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
    ColorForeground foreground;
    ColorBackground background;
};
#endif
