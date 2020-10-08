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
    std::string file;
    Editor editor;
    if (argc > 1) {
        file = argv[1];
        editor.set_file(file);
    } else {
        editor.mode = Mode::EXIT;
    }
    initialize_ncurses();
    while (editor.mode != Mode::EXIT) {
        editor.print_buffer();
        editor.handle_input(getch());
    }
    refresh();
    endwin();
    return 0;
}
