#include "interface.hpp"

#ifndef TEST
#include <ncurses.h>
#endif

#include <string>

#include "color.hpp"

#ifdef TEST
Interface::Interface() : lines(0), columns(0) {}
#else
Interface::Interface() : lines(LINES), columns(COLS) {}
#endif

void Interface::update() {
#ifdef TEST
    const int MOCK_SIZE = 50;
    lines = MOCK_SIZE;
    columns = MOCK_SIZE;
#else
    lines = LINES;
    columns = COLS;
#endif
}

int Interface::refresh() {
#ifdef TEST
    return 1;
#else
    return wrefresh(stdscr);
#endif
}

int Interface::cursor_set(int visibility) {
#ifdef TEST
    return 1;
#else
    return curs_set(visibility);
#endif
}

int Interface::move_cursor(int y, int x) {
#ifdef TEST
    return 1;
#else
    return move(y, x);
#endif
}

int Interface::mv_print(int y, int x, const std::string &str) {
#ifdef TEST
    return 1;
#else
    return mvprintw(y, x, "%s", str.c_str());
#endif
}

int Interface::mv_print_ch(int y, int x, char c) {
#ifdef TEST
    return 1;
#else
    return mvaddch(y, x, c);
#endif
}

int Interface::clear_to_eol() {
#ifdef TEST
    return 1;
#else
    return clrtoeol();
#endif
}

int Interface::attribute_on(short color_pair) {
#ifdef TEST
    return 1;
#else
    return attron(COLOR_PAIR(color_pair));
#endif
}

int Interface::attribute_off(short color_pair) {
#ifdef TEST
    return 1;
#else
    return attroff(COLOR_PAIR(color_pair));
#endif
}

int Interface::get_current_y() {
#ifdef TEST
    return 1;
#else
    return getcury(stdscr);
#endif
}

int Interface::get_current_x() {
#ifdef TEST
    return 1;
#else
    return getcurx(stdscr);
#endif
}

int Interface::get_input() {
#ifdef TEST
    return 1;
#else
    return getch();
#endif
}

int Interface::initialize_color(short &color_number, Color color) {
#ifdef TEST
    int result = 1;
#else
    int result = init_color(color_number, color.r, color.g, color.b);
#endif
    ++color_number;
    return result;
}

bool Interface::has_color_capability() {
#ifdef TEST
    return 1;
#else
    return has_colors();
#endif
}
