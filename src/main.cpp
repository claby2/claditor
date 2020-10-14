#include <ncurses.h>

#include <array>
#include <string>

#include "buffer.hpp"
#include "color.hpp"
#include "editor.hpp"

const size_t COLORS_DEFINED = 10;
std::array<Color, COLORS_DEFINED> default_colors;

void initialize_ncurses() {
    initscr();
    noecho();
    cbreak();
    raw();
    if (has_colors()) {
        start_color();
        for (size_t i = 0; i < COLORS_DEFINED; ++i) {
            short index = i;
            Color color;
            color_content(index, &color.r, &color.g, &color.b);
            init_color(index, color.r, color.g, color.b);
            default_colors[i] = color;
        }
    }
    keypad(stdscr, true);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string file = argv[1];
        Editor editor;
        editor.set_file(file);
        initialize_ncurses();
        editor.main();
        refresh();
        if (has_colors()) {
            // Reset colors
            for (size_t i = 0; i < COLORS_DEFINED; ++i) {
                init_color(static_cast<short>(i), default_colors[i].r,
                           default_colors[i].g, default_colors[i].b);
            }
        }
        endwin();
    }
    return 0;
}
