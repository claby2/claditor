#include "interface.hpp"

#ifndef UNIT_TEST
#include <ncurses.h>
#endif

#include <string>
#include <vector>

#include "color.hpp"

Interface::Interface() : lines(0), columns(0) { update(); }

void Interface::update() {
#ifndef UNIT_TEST
    lines = LINES;
    columns = COLS;
#endif
}

int Interface::refresh() {
#ifdef UNIT_TEST
    return 1;
#else
    return wrefresh(stdscr);
#endif
}

int Interface::cursor_set(int visibility) {
#ifdef UNIT_TEST
    return 1;
#else
    return curs_set(visibility);
#endif
}

int Interface::move_cursor(int y, int x) {
#ifdef UNIT_TEST
    return 1;
#else
    return move(y, x);
#endif
}

int Interface::mv_print(int y, int x, const std::string &str) {
#ifdef UNIT_TEST
    return 1;
#else
    return mvprintw(y, x, "%s", str.c_str());
#endif
}

int Interface::mv_print_ch(int y, int x, char c) {
#ifdef UNIT_TEST
    return 1;
#else
    return mvaddch(y, x, c);
#endif
}

int Interface::clear_to_eol() {
#ifdef UNIT_TEST
    return 1;
#else
    return clrtoeol();
#endif
}

int Interface::attribute_on(short color_pair) {
#ifdef UNIT_TEST
    return 1;
#else
    return attron(COLOR_PAIR(color_pair));
#endif
}

int Interface::attribute_off(short color_pair) {
#ifdef UNIT_TEST
    return 1;
#else
    return attroff(COLOR_PAIR(color_pair));
#endif
}

int Interface::get_current_y() {
#ifdef UNIT_TEST
    return 1;
#else
    return getcury(stdscr);
#endif
}

int Interface::get_current_x() {
#ifdef UNIT_TEST
    return 1;
#else
    return getcurx(stdscr);
#endif
}

int Interface::get_input() {
#ifdef UNIT_TEST
    int result = 0;
    if (current_input_ < inputs_.size()) {
        result = inputs_[current_input_];
        ++current_input_;
    }
    return result;
#else
    return getch();
#endif
}

int Interface::initialize_color(short &color_number, Color color) {
    int result = 0;
#ifdef UNIT_TEST
    result = 1;
#else
    result = init_color(color_number, color.r, color.g, color.b);
#endif
    ++color_number;
    return result;
}

bool Interface::has_color_capability() {
#ifdef UNIT_TEST
    return 1;
#else
    return has_colors();
#endif
}

#ifdef UNIT_TEST
void Interface::set_inputs(const std::vector<int> &inputs) {
    current_input_ = 0;
    inputs_ = inputs;
}

void Interface::set_dimensions(int lines, int columns) {
    this->lines = lines;
    this->columns = columns;
}
#endif
