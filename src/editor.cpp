#include <ncurses.h>

#include <algorithm>
#include <fstream>
#include <string>

#include "buffer.hpp"
#include "editor.hpp"
#include "history.hpp"

Editor::Editor()
    : mode(Mode::NORMAL),
      x_(0),
      y_(0),
      saved_x_(0),
      saved_y_(0),
      last_column_(0),
      file_started_empty(false) {}

void Editor::set_file(std::string file_path) {
    file_path_ = file_path;
    std::ifstream file;
    file.open(file_path.c_str(), std::ios::in);
    std::string line;
    while (std::getline(file, line)) {
        buffer_.push_back_line(line);
    }
    if (buffer_.get_size() == 0) {
        // File is empty
        // Add an empty line to prevent segmentation fault when accessing buffer
        file_started_empty = true;
        buffer_.push_back_line("");
    }
    history_.set(buffer_.lines);
}

void Editor::handle_input(int input) {
    switch (input) {
        case 27:  // ESC
            // Go to insert mode
            switch (mode) {
                case Mode::COMMAND:
                    exit_command_mode();
                    break;
                case Mode::INSERT:
                    exit_insert_mode();
                    break;
                default:
                    break;
            }
            return;
    }
    switch (mode) {
        case Mode::NORMAL:
            switch (input) {
                case 'h':
                    move_left();
                    break;
                case 'j':
                    move_down();
                    break;
                case 'k':
                    move_up();
                    break;
                case 'l':
                    move_right();
                    break;
                case 'i':
                    mode = Mode::INSERT;
                    break;
                case ':':
                    saved_x_ = x_;
                    saved_y_ = y_;
                    command_line_ = "";
                    mode = Mode::COMMAND;
                    break;
            }
            break;
        case Mode::INSERT:
            switch (input) {
                case 127:  // Backspace key
                    if (x_ == 0 && y_ > 0) {
                        x_ = buffer_.get_line_length(y_ - 1);
                        buffer_.add_string_to_line(buffer_.lines[y_], y_ - 1);
                        buffer_.remove_line(y_);
                        --y_;
                    } else {
                        // Erase character
                        buffer_.erase(--x_, 1, y_);
                    }
                    break;
                case 10:  // Enter key
                    if (x_ < buffer_.lines[y_].length()) {
                        // Move substring down
                        int substring_length = buffer_.get_line_length(y_) - x_;
                        buffer_.insert_line(
                            buffer_.lines[y_].substr(x_, substring_length),
                            y_ + 1);
                        buffer_.erase(x_, substring_length, y_);
                    } else {
                        buffer_.insert_line("", y_ + 1);
                    }
                    x_ = 0;
                    ++y_;
                    break;
                default:
                    buffer_.insert_char(x_, 1, static_cast<char>(input), y_);
                    ++x_;
                    break;
            }
            break;
        case Mode::COMMAND:
            switch (input) {
                case 10:  // Enter key
                    exit_command_mode();
                    parse_command();
                    break;
                case 127:  // Delete key
                    if (command_line_.empty()) {
                        exit_command_mode();
                    } else {
                        command_line_.pop_back();
                    }
                    break;
                default:
                    command_line_ += static_cast<char>(input);
                    break;
            }
            break;
        default:
            return;
    }
}

void Editor::print_buffer() {
    for (int i = 0; i < LINES - 1; ++i) {
        if (i >= buffer_.get_size()) {
            move(i, 0);
        } else {
            mvprintw(i, 0, "%s", buffer_.lines[i].c_str());
        }
        clrtoeol();
    }
    move(y_, x_);
}

void Editor::print_command_line() const {
    if (mode == Mode::COMMAND) {
        mvprintw(LINES - 1, 0, "%s", (':' + command_line_).c_str());
        clrtoeol();
    } else if (command_line_.empty()) {
        // Clear command line if not in command mode
        move(LINES - 1, 0);
        clrtoeol();
        move(y_, x_);
    }
}

int Editor::get_adjusted_x() {
    // When the y position is changed the x position needs to be updated to
    // adjust for line length
    int line_length = buffer_.get_line_length(y_);
    return last_column_ >= line_length ? std::max(line_length - 1, 0)
                                       : last_column_;
}

void Editor::move_up() {
    if (y_ - 1 >= 0) {
        --y_;
        x_ = get_adjusted_x();
        move(y_, x_);
    }
}

void Editor::move_right() {
    if (x_ + 1 < COLS && x_ + 1 < buffer_.get_line_length(y_)) {
        ++x_;
        last_column_ = x_;
        move(y_, x_);
    }
}

void Editor::move_down() {
    if (y_ + 1 < LINES - 1 && y_ + 1 < buffer_.get_size()) {
        ++y_;
        x_ = get_adjusted_x();
        move(y_, x_);
    }
}

void Editor::move_left() {
    if (x_ - 1 >= 0) {
        --x_;
        last_column_ = x_;
        move(y_, x_);
    }
}

void Editor::save_file() {
    std::ofstream file;
    file.open(file_path_.c_str(), std::ios::out);
    if (!(file_started_empty && buffer_.get_size() == 1 &&
          buffer_.lines[0].empty())) {
        for (const std::string &line : buffer_.lines) {
            file << line << '\n';
        }
    }
    file.close();
    history_.set(buffer_.lines);
}

void Editor::exit_command_mode() {
    x_ = saved_x_;
    y_ = saved_y_;
    move(LINES - 1, 0);
    clrtoeol();
    move(y_, x_);
    mode = Mode::NORMAL;
}

void Editor::exit_insert_mode() {
    getyx(stdscr, y_, x_);
    if (x_ - 1 >= 0) {
        --x_;
    }
    last_column_ = x_;
    move(y_, x_);
    mode = Mode::NORMAL;
}

void Editor::print_error(const std::string &error) {
    command_line_ = error;
    mvprintw(LINES - 1, 0, "%s", ("ERROR: " + command_line_).c_str());
    clrtoeol();
    move(y_, x_);
}

void Editor::parse_command() {
    if (command_line_ == "w") {
        save_file();
    } else if (command_line_ == "wq") {
        save_file();
        mode = Mode::EXIT;
    } else if (command_line_ == "q") {
        if (history_.has_unsaved_changes(buffer_.lines)) {
            print_error("No write since last change");
        } else {
            mode = Mode::EXIT;
        }
    } else if (command_line_ == "q!") {
        mode = Mode::EXIT;
    } else {
        print_error("Not an editor command: " + command_line_);
    }
}
