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
      first_line_(0),
      current_line_(0),
      line_number_width_(0),
      file_started_empty_(false) {}

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
        file_started_empty_ = true;
        buffer_.push_back_line("");
    }
    history_.set(buffer_.lines);
}

void Editor::handle_input(int input) {
    switch (input) {
        case 27:  // ESC
            set_mode(Mode::NORMAL);
            return;
    }
    switch (mode) {
        case Mode::NORMAL:
            switch (normal_bind_buffer_.length()) {
                case 0:
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
                        case '0':
                            x_ = 0;
                            break;
                        case '^':
                            x_ = buffer_.get_first_non_blank(current_line_);
                            break;
                        case 'x':
                            buffer_.erase(x_, 1, current_line_);
                            break;
                        case 'G': {
                            int last_line = buffer_.get_size() - 1;
                            first_line_ = last_line - LINES + 2;
                            x_ = buffer_.get_first_non_blank(last_line);
                            y_ = LINES - 2;
                            break;
                        }
                        case 'a':
                            ++x_;
                            set_mode(Mode::INSERT);
                            break;
                        case 'A':
                            x_ = buffer_.get_line_length(current_line_);
                            set_mode(Mode::INSERT);
                            break;
                        case 'i':
                            set_mode(Mode::INSERT);
                            break;
                        case 'o':
                            buffer_.insert_line("", current_line_ + 1);
                            x_ = 0;
                            ++y_;
                            set_mode(Mode::INSERT);
                            break;
                        case 'O':
                            buffer_.insert_line("", current_line_);
                            x_ = 0;
                            set_mode(Mode::INSERT);
                            break;
                        case ':':
                            saved_x_ = x_;
                            saved_y_ = y_;
                            command_line_ = "";
                            set_mode(Mode::COMMAND);
                            break;
                        default:
                            normal_bind_buffer_ += static_cast<char>(input);
                            break;
                    }
                    break;
                case 1:
                    switch (input) {
                        case 'g':
                            if (normal_bind_buffer_ == "g") {  // Bind: gg
                                // Go to first line and first non-blank
                                // character
                                first_line_ = 0;
                                x_ = buffer_.get_first_non_blank(0);
                                y_ = 0;
                            }
                        case 'd':
                            if (normal_bind_buffer_ == "d") {  // Bind: dd
                                buffer_.remove_line(current_line_);
                                x_ = buffer_.get_first_non_blank(current_line_);
                            }
                    }
                    normal_bind_buffer_ = "";
                    break;
                default:
                    normal_bind_buffer_ = "";
            }
            break;
        case Mode::INSERT:
            switch (input) {
                case 127:  // Backspace key
                case KEY_BACKSPACE:
                    if (x_ == 0 && current_line_ > 0) {
                        x_ = buffer_.get_line_length(current_line_ - 1);
                        buffer_.add_string_to_line(buffer_.lines[current_line_],
                                                   current_line_ - 1);
                        buffer_.remove_line(current_line_);
                        --y_;
                    } else if (!(x_ == 0 && current_line_ == 0)) {
                        // Erase character
                        buffer_.erase(--x_, 1, current_line_);
                    }
                    break;
                case 10:  // Enter key
                case KEY_ENTER:
                    if (x_ < buffer_.lines[current_line_].length()) {
                        // Move substring down
                        int substring_length =
                            buffer_.get_line_length(current_line_) - x_;
                        buffer_.insert_line(buffer_.lines[current_line_].substr(
                                                x_, substring_length),
                                            current_line_ + 1);
                        buffer_.erase(x_, substring_length, current_line_);
                    } else {
                        buffer_.insert_line("", current_line_ + 1);
                    }
                    x_ = 0;
                    ++y_;
                    break;
                default:
                    buffer_.insert_char(x_, 1, static_cast<char>(input),
                                        current_line_);
                    ++x_;
                    break;
            }
            break;
        case Mode::COMMAND:
            switch (input) {
                case 10:  // Enter key
                    set_mode(Mode::NORMAL);
                    parse_command();
                    break;
                case 127:  // Delete key
                    if (command_line_.empty()) {
                        set_mode(Mode::NORMAL);
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
            std::string line_number = std::to_string(first_line_ + i + 1);
            std::string line_number_content =
                std::string(line_number_width_ - line_number.length(), ' ') +
                line_number;
            mvprintw(
                i, 0, "%s",
                (line_number_content + ' ' + buffer_.lines[first_line_ + i])
                    .c_str());
        }
        clrtoeol();
    }
    adjusted_move(y_, x_);
}

void Editor::print_command_line() {
    if (mode == Mode::COMMAND) {
        mvprintw(LINES - 1, 0, "%s", (':' + command_line_).c_str());
        clrtoeol();
    } else if (command_line_.empty()) {
        // Clear command line if not in command mode
        move(LINES - 1, 0);
        clrtoeol();
        adjusted_move(y_, x_);
    }
}

void Editor::update() {
    current_line_ = first_line_ + y_;
    line_number_width_ = std::to_string(buffer_.get_size() + 1).length() + 1;
}

int Editor::get_adjusted_x() {
    // When the y position is changed the x position needs to be updated to
    // adjust for line length
    int line_length = buffer_.get_line_length(first_line_ + y_);
    return last_column_ >= line_length ? std::max(line_length - 1, 0)
                                       : last_column_;
}

void Editor::adjusted_move(int y, int x) {
    // Move cursor with line number width offset
    move(y, line_number_width_ + x + 1);
}

void Editor::move_up() {
    if (current_line_ - 1 >= 0 && y_ - 1 >= 0) {
        --y_;
        x_ = get_adjusted_x();
    } else if (current_line_ - 1 >= 0) {
        --first_line_;
        x_ = get_adjusted_x();
    }
}

void Editor::move_right() {
    if (x_ + 1 < COLS && x_ + 1 < buffer_.get_line_length(current_line_)) {
        ++x_;
        last_column_ = x_;
    }
}

void Editor::move_down() {
    if (y_ + 1 < LINES - 1 && current_line_ + 1 < buffer_.get_size()) {
        ++y_;
        x_ = get_adjusted_x();
    } else if (current_line_ + 1 < buffer_.get_size()) {
        // Scroll down
        ++first_line_;
        x_ = get_adjusted_x();
    }
}

void Editor::move_left() {
    if (x_ - 1 >= 0) {
        --x_;
        last_column_ = x_;
    }
}

void Editor::save_file() {
    std::ofstream file;
    file.open(file_path_.c_str(), std::ios::out);
    if (!(file_started_empty_ && buffer_.get_size() == 1 &&
          buffer_.lines[0].empty())) {
        for (const std::string &line : buffer_.lines) {
            file << line << '\n';
        }
    }
    file.close();
    history_.set(buffer_.lines);
    print_message("\"" + file_path_ + "\" written");
}

void Editor::print_message(const std::string &message) {
    command_line_ = message;
    mvprintw(LINES - 1, 0, "%s", command_line_.c_str());
    clrtoeol();
    adjusted_move(y_, x_);
}

void Editor::print_error(const std::string &error) {
    print_message("ERROR: " + error);
}

void Editor::parse_command() {
    if (command_line_ == "w") {
        save_file();
    } else if (command_line_ == "wq") {
        save_file();
        set_mode(Mode::EXIT);
    } else if (command_line_ == "q") {
        if (history_.has_unsaved_changes(buffer_.lines)) {
            print_error("No write since last change");
        } else {
            set_mode(Mode::EXIT);
        }
    } else if (command_line_ == "q!") {
        set_mode(Mode::EXIT);
    } else {
        print_error("Not an editor command: " + command_line_);
    }
}

void Editor::exit_command_mode() {
    x_ = saved_x_;
    y_ = saved_y_;
    move(LINES - 1, 0);
    clrtoeol();
    move(y_, x_);
}

void Editor::exit_insert_mode() {
    int new_y;
    int new_x;
    getyx(stdscr, new_y, new_x);
    if (new_x - 1 >= line_number_width_ + 1) {
        --x_;
    }
    last_column_ = x_;
    adjusted_move(y_, x_);
    print_message("");
}

void Editor::exit_normal_mode() { normal_bind_buffer_ = ""; }

void Editor::set_mode(Mode new_mode) {
    if (mode != new_mode) {
        switch (mode) {
            case Mode::INSERT:
                exit_insert_mode();
                break;
            case Mode::COMMAND:
                exit_command_mode();
                break;
            case Mode::NORMAL:
                exit_normal_mode();
                break;
            default:
                break;
        }
        switch (new_mode) {
            case Mode::INSERT:
                print_message("-- INSERT --");
                break;
            default:
                break;
        }
        mode = new_mode;
    }
}
