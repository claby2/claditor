#include <dirent.h>
#include <ncurses.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <string>
#include <unordered_map>

#include "bind_count.hpp"
#include "buffer.hpp"
#include "color.hpp"
#include "editor.hpp"
#include "history.hpp"
#include "runtime.hpp"

enum class InputKey : int { ENTER = 10, ESCAPE = 27, BACKSPACE = 127 };

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

void Editor::set_file(const std::string &file_path) {
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
    file_path_ = file_path;
}

void Editor::main() {
    get_colorschemes();
    state_enter(&Editor::normal_state);
}

void Editor::print_buffer() {
    set_color(ColorType::DEFAULT);
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
    unset_color(ColorType::DEFAULT);
    adjusted_move(y_, x_);
}

void Editor::print_command_line() {
    set_color(ColorType::DEFAULT);
    if (mode == Mode::COMMAND) {
        mvprintw(LINES - 1, 0, "%s", (':' + command_line_).c_str());
        clrtoeol();
    } else if (command_line_.empty()) {
        // Clear command line if not in command mode
        move(LINES - 1, 0);
        clrtoeol();
        adjusted_move(y_, x_);
    }
    unset_color(ColorType::DEFAULT);
}

void Editor::update() {
    current_line_ = first_line_ + y_;
    line_number_width_ = std::to_string(buffer_.get_size() + 1).length() + 1;
}

bool Editor::normal_state(int input) {
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
            normal_first_char();
            break;
        case '^':
            normal_first_non_blank_char();
            break;
        case 'x':
            normal_delete();
            break;
        case 'g':
            state_enter(&Editor::normal_command_g_state);
            break;
        case 'G':
            if (normal_bind_count_.empty()) {
                normal_end_of_file();
            } else {
                normal_jump_line(normal_bind_count_.get_value() - 1);
                x_ = buffer_.get_first_non_blank(first_line_ + y_);
                last_column_ = x_;
            }
            break;
        case 'a':
            normal_append_after_cursor();
            break;
        case 'A':
            normal_append_end_of_line();
            break;
        case 'i':
            set_mode(Mode::INSERT);
            state_enter(&Editor::insert_state);
            break;
        case 'o':
            normal_begin_new_line_below();
            break;
        case 'O':
            normal_begin_new_line_above();
            break;
        case 'd':
            state_enter(&Editor::normal_command_d_state);
            break;
        case ':':
            saved_x_ = x_;
            saved_y_ = y_;
            command_line_ = "";
            set_mode(Mode::COMMAND);
            state_enter(&Editor::command_state);
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            normal_add_count(input);
            state_enter(&Editor::normal_add_count_state);
            break;
        default:
            normal_bind_count_.reset();
            break;
    }
    return true;
}

bool Editor::insert_state(int input) {
    switch (input) {
        case static_cast<int>(InputKey::ESCAPE):
            set_mode(Mode::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        case static_cast<int>(InputKey::BACKSPACE):
        case KEY_BACKSPACE:
            insert_backspace();
            break;
        case static_cast<int>(InputKey::ENTER):
        case KEY_ENTER:
            insert_enter();
            break;
        default:
            insert_char(input);
            break;
    }
    return true;
}

bool Editor::command_state(int input) {
    switch (input) {
        case static_cast<int>(InputKey::ESCAPE):
            set_mode(Mode::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        case static_cast<int>(InputKey::BACKSPACE):
        case KEY_BACKSPACE:
            command_backspace();
            break;
        case static_cast<int>(InputKey::ENTER):
        case KEY_ENTER:
            command_enter();
            break;
        default:
            command_char(input);
            break;
    }
    return true;
}

void Editor::state_enter(bool (Editor::*state_callback)(int)) {
    int input = 0;
    do {
        update();
        print_buffer();
        print_command_line();
        input = getch();
    } while ((this->*state_callback)(input) && mode != Mode::EXIT);
}

void Editor::normal_first_char() { x_ = 0; }

void Editor::normal_first_non_blank_char() {
    x_ = buffer_.get_first_non_blank(current_line_);
}

void Editor::normal_delete() {
    buffer_.erase(x_, 1, current_line_);
    int current_line_length = buffer_.get_line_length(current_line_);
    if (x_ >= current_line_length) {
        x_ = current_line_length - 1;
    }
}

void Editor::normal_end_of_file() {
    int last_line = buffer_.get_size() - 1;
    if (buffer_.get_size() > LINES - 1) {
        first_line_ = last_line - LINES + 2;
        y_ = LINES - 2;
    } else {
        first_line_ = 0;
        y_ = last_line;
    }
    x_ = buffer_.get_first_non_blank(last_line);
    last_column_ = x_;
}

void Editor::normal_append_after_cursor() {
    ++x_;
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_append_end_of_line() {
    x_ = buffer_.get_line_length(current_line_);
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_begin_new_line_below() {
    buffer_.insert_line("", current_line_ + 1);
    x_ = 0;
    ++y_;
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_begin_new_line_above() {
    buffer_.insert_line("", current_line_);
    x_ = 0;
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_first_line() {
    first_line_ = 0;
    x_ = buffer_.get_first_non_blank(0);
    y_ = 0;
}

void Editor::normal_jump_line(int line) {
    // Ensure that line exists in buffer
    line = std::max(0, std::min(buffer_.get_size() - 1, line));
    if (line >= first_line_ && line < first_line_ + LINES - 1) {
        // Target line is already visible
        // There is no need to change first_line_
        y_ = line - first_line_;
    } else if (std::abs(line - (first_line_ + std::floor(LINES / 2))) > LINES) {
        // Make y_ middle point of screen
        first_line_ = std::max(
            0, std::min(buffer_.get_size() - LINES + 1,
                        static_cast<int>(line - std::floor(LINES / 2))));
        y_ = line - first_line_;
    } else if (line < current_line_) {
        first_line_ = line;
        y_ = 0;
    } else {
        first_line_ = line - LINES + 2;
        y_ = LINES - 2;
    }
}

void Editor::normal_delete_line() {
    if (buffer_.get_size() > 1) {
        buffer_.remove_line(current_line_);
        x_ = buffer_.get_first_non_blank(current_line_);
    } else {
        buffer_.set_line("", 0);
        x_ = 0;
    }
}

void Editor::normal_add_count(int input) {
    // Input represents char, convert to integer
    normal_bind_count_.add_digit(input - '0');
}

bool Editor::normal_command_g_state(int input) {
    switch (input) {
        case 'g':  // Bind: gg
            if (normal_bind_count_.empty()) {
                normal_first_line();
                last_column_ = x_;
            } else {
                normal_jump_line(normal_bind_count_.get_value() - 1);
                x_ = buffer_.get_first_non_blank(first_line_ + y_);
                last_column_ = x_;
            }
            break;
    }
    return false;
}

bool Editor::normal_command_d_state(int input) {
    switch (input) {
        case 'd':  // Bind: dd
            normal_delete_line();
            break;
    }
    return false;
}

bool Editor::normal_add_count_state(int input) {
    switch (input) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            normal_add_count(input);
            state_enter(&Editor::normal_add_count_state);
            break;
        default:
            normal_state(input);
            break;
    }
    return false;
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
    if (normal_bind_count_.empty()) {
        if (current_line_ - 1 >= 0 && y_ - 1 >= 0) {
            --y_;
        } else if (current_line_ - 1 >= 0) {
            --first_line_;
        }
        x_ = get_adjusted_x();
    } else {
        // Move up by [count] lines
        normal_jump_line(current_line_ - normal_bind_count_.get_value());
        x_ = get_adjusted_x();
    }
}

void Editor::move_right() {
    if (normal_bind_count_.empty()) {
        if (x_ + 1 < COLS && x_ + 1 < buffer_.get_line_length(current_line_)) {
            ++x_;
            last_column_ = x_;
        }
    } else {
        // Move right by [count] characters
        x_ = std::min(buffer_.get_line_length(current_line_) - 1,
                      x_ + normal_bind_count_.get_value());
        last_column_ = x_;
    }
}

void Editor::move_down() {
    if (normal_bind_count_.empty()) {
        if (y_ + 1 < LINES - 1 && current_line_ + 1 < buffer_.get_size()) {
            ++y_;
        } else if (current_line_ + 1 < buffer_.get_size()) {
            // Scroll down
            ++first_line_;
        }
        x_ = get_adjusted_x();
    } else {
        // Move down by [count] lines
        normal_jump_line(current_line_ + normal_bind_count_.get_value());
        x_ = get_adjusted_x();
    }
}

void Editor::move_left() {
    if (normal_bind_count_.empty()) {
        if (x_ - 1 >= 0) {
            --x_;
            last_column_ = x_;
        }
    } else {
        // Move left by [count] characters
        x_ = std::max(0, x_ - normal_bind_count_.get_value());
        last_column_ = x_;
    }
}

void Editor::insert_backspace() {
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
}

void Editor::insert_enter() {
    if (x_ < buffer_.lines[current_line_].length()) {
        // Move substring down
        int substring_length = buffer_.get_line_length(current_line_) - x_;
        buffer_.insert_line(
            buffer_.lines[current_line_].substr(x_, substring_length),
            current_line_ + 1);
        buffer_.erase(x_, substring_length, current_line_);
    } else {
        buffer_.insert_line("", current_line_ + 1);
    }
    x_ = 0;
    ++y_;
}

void Editor::insert_char(int input) {
    buffer_.insert_char(x_, 1, static_cast<char>(input), current_line_);
    ++x_;
}

void Editor::command_backspace() {
    if (command_line_.empty()) {
        set_mode(Mode::NORMAL);
        state_enter(&Editor::insert_state);
    } else {
        command_line_.pop_back();
    }
}

void Editor::command_enter() {
    set_mode(Mode::NORMAL);
    parse_command();
    if (mode != Mode::EXIT) {
        state_enter(&Editor::normal_state);
    }
}

void Editor::command_char(int input) {
    command_line_ += static_cast<char>(input);
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

void Editor::get_colorschemes() {
    // Get colorschemes
    // Iterate through the runtime colors directory and push back colorschemes
    std::string home_directory = get_home_directory();
    std::string colors_directory = home_directory + "/.config/claditor/colors/";
    DIR *dir;
    dirent entry;
    dirent *result;
    if ((dir = opendir(colors_directory.c_str())) != 0) {
        while (readdir_r(dir, &entry, &result) == 0 && result != 0) {
            std::string file_name = entry.d_name;
            if (file_name.substr(file_name.find_last_of(".") + 1) == "clad") {
                // File has .clad file extension
                // Parse the file
                std::string colorscheme_name =
                    file_name.substr(0, file_name.find_last_of("."));
                Colorscheme colorscheme(colors_directory + file_name);
                colorschemes_[colorscheme_name] = colorscheme;
            }
        }
        closedir(dir);
    }
}

void Editor::set_colorscheme(const std::string &new_colorscheme_name) {
    colorscheme_name_ = new_colorscheme_name;
    colorscheme_ = colorschemes_[colorscheme_name_];
    // Initialize ncurses colors
    short current_color = 0;
    auto initialize_color = [&current_color](Color color) {
        init_color(current_color, color.r, color.g, color.b);
        if (current_color != 0) {
            init_pair(current_color, current_color, 0);
        }
        ++current_color;
    };
    initialize_color(colorscheme_.background);
    initialize_color(colorscheme_.foreground);
    initialize_color(colorscheme_.comment);
    initialize_color(colorscheme_.accent);
    initialize_color(colorscheme_.color1);
    initialize_color(colorscheme_.color2);
    initialize_color(colorscheme_.color3);
    initialize_color(colorscheme_.color4);
    initialize_color(colorscheme_.color5);
    initialize_color(colorscheme_.color6);
}

void Editor::set_color(ColorType color) const {
    if (has_colors() && !colorscheme_name_.empty()) {
        attron(COLOR_PAIR(static_cast<short>(color)));
    }
}

void Editor::unset_color(ColorType color) const {
    if (has_colors() && !colorscheme_name_.empty()) {
        attroff(COLOR_PAIR(static_cast<short>(color)));
    }
}

void Editor::print_message(const std::string &message) {
    set_color(ColorType::DEFAULT);
    mvprintw(LINES - 1, 0, "%s", message.c_str());
    clrtoeol();
    adjusted_move(y_, x_);
    unset_color(ColorType::DEFAULT);
}

void Editor::print_error(const std::string &error) {
    set_color(ColorType::COLOR1);
    mvprintw(LINES - 1, 0, "ERROR: %s", error.c_str());
    clrtoeol();
    adjusted_move(y_, x_);
    unset_color(ColorType::COLOR1);
}

void Editor::parse_command() {
    std::string command = command_line_;
    std::string args = "";
    std::string::size_type space_delimiter = command_line_.find(" ");
    // If has_space_delimiter is true the given command may have additional
    // arguments
    if (space_delimiter != std::string::npos) {
        command = command_line_.substr(0, space_delimiter);
        args = command_line_.substr(space_delimiter + 1);
    }
    if (command == "w") {
        save_file();
    } else if (command == "wq") {
        save_file();
        set_mode(Mode::EXIT);
    } else if (command == "q") {
        if (history_.has_unsaved_changes(buffer_.lines)) {
            print_error("No write since last change");
        } else {
            set_mode(Mode::EXIT);
        }
    } else if (command == "q!") {
        set_mode(Mode::EXIT);
    } else if (command == "colorscheme" || command == "colo") {
        // If no arguments are given, print the current colorscheme name
        // Otherwise, set the current colorscheme to the argument
        if (args.empty() && colorscheme_name_.empty()) {
            print_error("No colorscheme detected");
        } else if (args.empty()) {
            print_message(colorscheme_name_);
        } else if (colorschemes_.find(args) != colorschemes_.end()) {
            set_colorscheme(args);
        } else {
            print_error("Cannot find colorscheme '" + args + "'");
        }
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
    int new_y = 0;
    int new_x = 0;
    getyx(stdscr, new_y, new_x);
    if (new_x - 1 >= line_number_width_ + 1) {
        --x_;
    }
    last_column_ = x_;
    adjusted_move(y_, x_);
    print_message("");
}

void Editor::exit_normal_mode() { normal_bind_count_.reset(); }

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
