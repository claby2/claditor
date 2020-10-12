#ifndef CLADITOR_COLOR_HPP
#define CLADITOR_COLOR_HPP

#include <string>

struct Color {
    Color();
    Color(short, short, short);

    short r;
    short g;
    short b;
};

struct Colorscheme {
    Colorscheme();
    explicit Colorscheme(std::string);

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
#endif
