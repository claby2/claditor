#include <ncurses.h>

#include <algorithm>
#include <fstream>
#include <string>

#include "buffer.hpp"
#include "editor.hpp"

Editor::Editor() : mode(Mode::NORMAL), x_(0), y_(0) {}

void Editor::set_file(std::string file_path) {
    file_path_ = file_path;
    file_.open(file_path.c_str(), std::ios::in);
    std::string line;
    while (std::getline(file_, line)) {
        buffer_.lines.push_back(line);
    }
}

void Editor::handle_input(int input) {
    if (static_cast<char>(input) == 'q') {
        save_file();
        mode = Mode::EXIT;
    }
}

void Editor::print_buffer() {
    for (int i = 0; i < LINES; ++i) {
        if (i >= buffer_.lines.size()) {
            move(i, 0);
        } else {
            mvprintw(i, 0, buffer_.lines[i].c_str());
        }
        clrtoeol();
    }
    move(x_, y_);
}

void Editor::move_up() {
    if (y_ - 1 >= 0) {
        --y_;
        int previous_line_length = buffer_.lines[y_].length();
        x_ = x_ >= previous_line_length ? previous_line_length : x_;
        move(y_, x_);
    }
}

void Editor::move_right() {
    if (x_ + 1 > COLS && x_ + 1 <= buffer_.lines[y_].length()) {
        ++x_;
        move(y_, x_);
    }
}

void Editor::move_down() {
    if (y_ + 1 <= LINES && y_ + 1 < buffer_.lines.size()) {
        ++y_;
    }
}

void Editor::move_left() {
    if (x_ - 1 >= 0) {
        --x_;
        int previous_line_length = buffer_.lines[y_].length();
        x_ = x_ >= previous_line_length ? previous_line_length : x_;
        move(y_, x_);
    }
}

void Editor::save_file() {
    file_.open(file_path_.c_str(), std::ios::out);
    for (const std::string &line : buffer_.lines) {
        file_ << line << '\n';
    }
    file_.close();
}
