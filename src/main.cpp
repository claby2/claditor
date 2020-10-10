#include <ncurses.h>

#include <string>

#include "buffer.hpp"
#include "editor.hpp"

void initialize_ncurses() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string file = argv[1];
        Editor editor;
        editor.set_file(file);
        initialize_ncurses();
        editor.main();
        refresh();
        endwin();
    }
    return 0;
}
