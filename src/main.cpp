#include <ncurses.h>

#include <string>
#include <utility>
#include <vector>

#include "color.hpp"
#include "editor.hpp"

std::vector<Color> default_colors(COLORS_DEFINED);
std::vector<std::pair<short, short>> default_pairs(PAIRS_DEFINED);

std::pair<short, short> get_color_pair(short color_pair) {
    // Return color pair content given color pair index
    short foreground = 0;
    short background = 0;
    pair_content(color_pair, &foreground, &background);
    return std::make_pair(foreground, background);
}

void initialize_ncurses() {
    initscr();
    noecho();
    cbreak();
    raw();
    if (has_colors()) {
        start_color();
        // Backup and initialize colors
        for (size_t i = 0; i < COLORS_DEFINED; ++i) {
            short index = i;
            Color color;
            color_content(index, &color.r, &color.g, &color.b);

            // Store color to restore later
            default_colors[i] = color;

            // Initialize color
            init_color(index, color.r, color.g, color.b);
        }

        // Backup and initialize color pairs
        for (short i = 1; i < PAIRS_DEFINED + 1; ++i) {
            default_pairs[i - 1] = get_color_pair(i);
            short foreground =
                i < COLORS_DEFINED
                    ? static_cast<short>(i)
                    : static_cast<short>((i % COLORS_DEFINED) + 1);
            short background =
                i < COLORS_DEFINED
                    ? 0
                    : static_cast<short>(ColorForeground::ACCENT);
            init_pair(i, foreground, background);
        }
    }
    keypad(stdscr, true);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string file = argv[1];
        Editor editor(file);
        initialize_ncurses();
        editor.start();
        refresh();
        if (has_colors()) {
            // Restore colors
            for (size_t i = 0; i < COLORS_DEFINED; ++i) {
                init_color(static_cast<short>(i), default_colors[i].r,
                           default_colors[i].g, default_colors[i].b);
            }

            // Restore color pairs
            for (short i = 1; i < PAIRS_DEFINED + 1; ++i) {
                std::pair<short, short> color_pair = default_pairs[i - 1];
                init_pair(static_cast<short>(i), color_pair.first,
                          color_pair.second);
            }
        }
        endwin();
    }
    return 0;
}
