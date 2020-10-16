#include "editor.hpp"

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
#include "command.hpp"
#include "history.hpp"
#include "runtime.hpp"

enum class InputKey : int { ENTER = 10, ESCAPE = 27, BACKSPACE = 127 };

Position::Position() : y(0), x(0) {}

Position::Position(int new_y, int new_x) : y(new_y), x(new_x) {}

Editor::Editor()
    : mode(Mode::NORMAL),
      cursor_position_(0, 0),
      saved_position_(0, 0),
      visual_position_(0, 0),
      last_column_(0),
      first_line_(0),
      previous_first_line_(0),
      current_line_(0),
      line_number_width_(0),
      current_color_pair_{ColorForeground::DEFAULT, ColorBackground::DEFAULT},
      zero_lines_(false) {}

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
        zero_lines_ = true;
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
    bool has_scroll = previous_first_line_ != first_line_;
    if (has_scroll) {
        curs_set(0);
    }
    set_color(ColorForeground::DEFAULT, ColorBackground::DEFAULT);
    ColorPair default_color_pair = current_color_pair_;
    for (int i = 0; i < LINES - 1; ++i) {
        if (i >= buffer_.get_size()) {
            move(i, 0);
        } else {
            std::string line_number = std::to_string(first_line_ + i + 1);
            std::string line_number_content =
                std::string(line_number_width_ - line_number.length(), ' ') +
                line_number;
            std::string line = buffer_.lines[first_line_ + i];
            // Print line number
            mvprintw(i, 0, "%s", (line_number_content + ' ').c_str());
            // Print characters one by one
            for (size_t j = 0; j < line.length(); ++j) {
                bool visual_highlight = needs_visual_highlight(i, j);
                if (visual_highlight) {
                    unset_color();
                    set_color(ColorForeground::DEFAULT,
                              ColorBackground::ACCENT);
                }
                mvaddch(i, line_number_width_ + 1 + j, line[j]);
                if (visual_highlight) {
                    unset_color();
                    set_color(default_color_pair.foreground,
                              default_color_pair.background);
                }
            }
        }
        if (has_scroll) {
            refresh();
        }
        clrtoeol();
    }
    unset_color();
    adjusted_move(cursor_position_.y, cursor_position_.x);
    if (has_scroll) {
        curs_set(1);
    }
    previous_first_line_ = first_line_;
}

void Editor::print_command_line() {
    set_color(ColorForeground::DEFAULT, ColorBackground::DEFAULT);
    if (mode == Mode::COMMAND) {
        mvprintw(LINES - 1, 0, "%s", (':' + command_line_).c_str());
        clrtoeol();
    }
    unset_color();
}

void Editor::clear_command_line() {
    command_line_ = "";
    move(LINES - 1, 0);
    clrtoeol();
    adjusted_move(cursor_position_.y, cursor_position_.x);
}

void Editor::update() {
    if (zero_lines_ &&
        (buffer_.get_size() != 1 || buffer_.get_line_length(0) > 0)) {
        zero_lines_ = false;
    }
    current_line_ = first_line_ + cursor_position_.y;
    line_number_width_ = std::to_string(buffer_.get_size() + 1).length() + 1;
    refresh();
}

Position Editor::get_visual_start_position() {
    // Return position of the start of visual selection
    if (cursor_position_.y == visual_position_.y) {
        return {cursor_position_.y,
                std::min(cursor_position_.x, visual_position_.x)};
    }
    if (cursor_position_.y < visual_position_.y) {
        return cursor_position_;
    }
    return visual_position_;
}

Position Editor::get_visual_end_position() {
    // Return position of the end of visual selection
    if (cursor_position_.y == visual_position_.y) {
        return {cursor_position_.y,
                std::max(cursor_position_.x, visual_position_.x)};
    }
    if (cursor_position_.y < visual_position_.y) {
        return visual_position_;
    }
    return cursor_position_;
}

bool Editor::needs_visual_highlight(int y, int x) {
    if (mode != Mode::VISUAL ||
        (y == cursor_position_.y && x == cursor_position_.x)) {
        return false;
    }
    Position start = get_visual_start_position();
    Position end = get_visual_end_position();
    if (y > start.y && y < end.y) {
        return true;
    }
    if (start.y == end.y) {
        return x >= start.x && x <= end.x && y == start.y;
    }
    if (y == start.y) {
        return x >= start.x;
    }
    if (y == end.y) {
        return x <= end.x;
    }
    return false;
}

void Editor::normal_and_visual(int input) {
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
            last_column_ = cursor_position_.x;
            break;
        case '^':
            normal_first_non_blank_char();
            last_column_ = cursor_position_.x;
            break;
        case 'x':
            normal_delete();
            break;
        case 'g':
            state_enter(&Editor::normal_command_g_state);
            break;
        case 'G':
            if (bind_count_.empty()) {
                normal_end_of_file();
            } else {
                normal_jump_line(bind_count_.get_value() - 1);
                cursor_position_.x = buffer_.get_first_non_blank(
                    first_line_ + cursor_position_.y);
                last_column_ = cursor_position_.x;
            }
            break;
        case ':':
            saved_position_.x = cursor_position_.x;
            saved_position_.y = cursor_position_.y;
            clear_command_line();
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
            bind_count_.reset();
            break;
    }
}

bool Editor::normal_state(int input) {
    switch (input) {
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
        case 'v':
            set_mode(Mode::VISUAL);
            state_enter(&Editor::visual_state);
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
        default:
            normal_and_visual(input);
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

bool Editor::visual_state(int input) {
    switch (input) {
        case 'v':
        case static_cast<int>(InputKey::ESCAPE):
            set_mode(Mode::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        case 'd':
            visual_delete_selection();
            set_mode(Mode::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        default:
            normal_and_visual(input);
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

void Editor::normal_first_char() { cursor_position_.x = 0; }

void Editor::normal_first_non_blank_char() {
    cursor_position_.x = buffer_.get_first_non_blank(current_line_);
}

void Editor::normal_delete() {
    if (buffer_.get_line_length(current_line_) > 0) {
        buffer_.erase(cursor_position_.x, 1, current_line_);
        int current_line_length = buffer_.get_line_length(current_line_);
        if (cursor_position_.x >= current_line_length &&
            current_line_length > 0) {
            cursor_position_.x = current_line_length - 1;
        }
    }
}

void Editor::normal_end_of_file() {
    int last_line = buffer_.get_size() - 1;
    if (buffer_.get_size() > LINES - 1) {
        first_line_ = last_line - LINES + 2;
        cursor_position_.y = LINES - 2;
    } else {
        first_line_ = 0;
        cursor_position_.y = last_line;
    }
    cursor_position_.x = buffer_.get_first_non_blank(last_line);
    last_column_ = cursor_position_.x;
}

void Editor::normal_append_after_cursor() {
    ++cursor_position_.x;
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_append_end_of_line() {
    cursor_position_.x = buffer_.get_line_length(current_line_);
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_begin_new_line_below() {
    buffer_.insert_line("", current_line_ + 1);
    cursor_position_.x = 0;
    ++cursor_position_.y;
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_begin_new_line_above() {
    buffer_.insert_line("", current_line_);
    cursor_position_.x = 0;
    set_mode(Mode::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_first_line() {
    first_line_ = 0;
    cursor_position_.x = buffer_.get_first_non_blank(0);
    cursor_position_.y = 0;
}

void Editor::normal_jump_line(int line) {
    // Ensure that line exists in buffer
    line = std::max(0, std::min(buffer_.get_size() - 1, line));
    if (line >= first_line_ && line < first_line_ + LINES - 1) {
        // Target line is already visible
        // There is no need to change first_line_
        cursor_position_.y = line - first_line_;
    } else if (std::abs(line - (first_line_ + std::floor(LINES / 2))) > LINES) {
        // Make cursor_position_.y middle point of screen
        first_line_ = std::max(
            0, std::min(buffer_.get_size() - LINES + 1,
                        static_cast<int>(line - std::floor(LINES / 2))));
        cursor_position_.y = line - first_line_;
    } else if (line < current_line_) {
        first_line_ = line;
        cursor_position_.y = 0;
    } else {
        first_line_ = line - LINES + 2;
        cursor_position_.y = LINES - 2;
    }
}

void Editor::normal_delete_line() {
    if (buffer_.get_size() > 1) {
        buffer_.remove_line(current_line_);
        cursor_position_.x = buffer_.get_first_non_blank(current_line_);
    } else {
        zero_lines_ = true;
        buffer_.set_line("", 0);
        cursor_position_.x = 0;
    }
}

void Editor::normal_add_count(int input) {
    // Input represents char, convert to integer
    bind_count_.add_digit(input - '0');
}

bool Editor::normal_command_g_state(int input) {
    switch (input) {
        case 'g':  // Bind: gg
            if (bind_count_.empty()) {
                normal_first_line();
                last_column_ = cursor_position_.x;
            } else {
                normal_jump_line(bind_count_.get_value() - 1);
                cursor_position_.x = buffer_.get_first_non_blank(
                    first_line_ + cursor_position_.y);
                last_column_ = cursor_position_.x;
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
    int line_length = buffer_.get_line_length(first_line_ + cursor_position_.y);
    return last_column_ >= line_length ? std::max(line_length - 1, 0)
                                       : last_column_;
}

void Editor::adjusted_move(int y, int x) {
    // Move cursor with line number width offset
    move(y, line_number_width_ + x + 1);
}

void Editor::move_up() {
    if (bind_count_.empty()) {
        if (current_line_ - 1 >= 0 && cursor_position_.y - 1 >= 0) {
            --cursor_position_.y;
        } else if (current_line_ - 1 >= 0) {
            --first_line_;
        }
        cursor_position_.x = get_adjusted_x();
    } else {
        // Move up by [count] lines
        normal_jump_line(current_line_ - bind_count_.get_value());
        cursor_position_.x = get_adjusted_x();
    }
}

void Editor::move_right() {
    if (bind_count_.empty()) {
        if (cursor_position_.x + 1 < COLS &&
            cursor_position_.x + 1 < buffer_.get_line_length(current_line_)) {
            ++cursor_position_.x;
            last_column_ = cursor_position_.x;
        }
    } else {
        // Move right by [count] characters
        cursor_position_.x =
            std::min(buffer_.get_line_length(current_line_) - 1,
                     cursor_position_.x + bind_count_.get_value());
        last_column_ = cursor_position_.x;
    }
}

void Editor::move_down() {
    if (bind_count_.empty()) {
        if (cursor_position_.y + 1 < LINES - 1 &&
            current_line_ + 1 < buffer_.get_size()) {
            ++cursor_position_.y;
        } else if (current_line_ + 1 < buffer_.get_size()) {
            // Scroll down
            ++first_line_;
        }
        cursor_position_.x = get_adjusted_x();
    } else {
        // Move down by [count] lines
        normal_jump_line(current_line_ + bind_count_.get_value());
        cursor_position_.x = get_adjusted_x();
    }
}

void Editor::move_left() {
    if (bind_count_.empty()) {
        if (cursor_position_.x - 1 >= 0) {
            --cursor_position_.x;
            last_column_ = cursor_position_.x;
        }
    } else {
        // Move left by [count] characters
        cursor_position_.x =
            std::max(0, cursor_position_.x - bind_count_.get_value());
        last_column_ = cursor_position_.x;
    }
}

void Editor::insert_backspace() {
    if (cursor_position_.x == 0 && current_line_ > 0) {
        cursor_position_.x = buffer_.get_line_length(current_line_ - 1);
        buffer_.add_string_to_line(buffer_.lines[current_line_],
                                   current_line_ - 1);
        buffer_.remove_line(current_line_);
        --cursor_position_.y;
    } else if (!(cursor_position_.x == 0 && current_line_ == 0)) {
        // Erase character
        buffer_.erase(--cursor_position_.x, 1, current_line_);
    }
}

void Editor::insert_enter() {
    if (cursor_position_.x < buffer_.lines[current_line_].length()) {
        // Move substring down
        int substring_length =
            buffer_.get_line_length(current_line_) - cursor_position_.x;
        buffer_.insert_line(buffer_.lines[current_line_].substr(
                                cursor_position_.x, substring_length),
                            current_line_ + 1);
        buffer_.erase(cursor_position_.x, substring_length, current_line_);
    } else {
        buffer_.insert_line("", current_line_ + 1);
    }
    cursor_position_.x = 0;
    ++cursor_position_.y;
}

void Editor::insert_char(int input) {
    buffer_.insert_char(cursor_position_.x, 1, static_cast<char>(input),
                        current_line_);
    ++cursor_position_.x;
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
    run_command();
    if (mode != Mode::EXIT) {
        state_enter(&Editor::normal_state);
    }
}

void Editor::command_char(int input) {
    command_line_ += static_cast<char>(input);
}

void Editor::visual_delete_selection() {
    Position start = get_visual_start_position();
    Position end = get_visual_end_position();
    if (start.y == end.y) {
        buffer_.erase(start.x, end.x - start.x + 1, start.y);
    } else {
        // Erase from start x to end of line
        buffer_.erase(start.x, buffer_.get_line_length(start.y), start.y);
        // Concatenate content at end line that will not be erased to the end of
        // start line
        // Only concatenate end content if there is at least one character
        bool concatenate_end_content = buffer_.get_line_length(end.y) > 0;
        if (concatenate_end_content) {
            std::string end_content =
                buffer_.lines[end.y].substr(end.x + 1, std::string::npos);
            buffer_.add_string_to_line(end_content, start.y);
        }
        buffer_.remove_line(end.y);
        // Delete lines between start and end lines
        for (int i = 0; i < end.y - start.y - 1; ++i) {
            buffer_.remove_line(start.y + 1);
        }
        if (!concatenate_end_content && start.x == 0) {
            // If there was no need to concatenate end content, remove the start
            // line
            buffer_.remove_line(start.y);
        }
        if (buffer_.get_size() == 0) {
            zero_lines_ = true;
            buffer_.push_back_line("");
        }
    }
    // Set cursor position to start of selection with adjustment if needed
    cursor_position_ = {
        start.y,
        std::max(0, std::min(buffer_.get_line_length(start.y) - 1, start.x))};
}

void Editor::save_file() {
    std::ofstream file;
    file.open(file_path_.c_str(), std::ios::out);
    if (!zero_lines_) {
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

void Editor::set_color(ColorForeground foreground, ColorBackground background) {
    if (has_colors() && !colorscheme_name_.empty()) {
        short color_pair = get_color_pair_index(foreground, background);
        attron(COLOR_PAIR(color_pair));
        current_color_pair_ = {foreground, background};
    }
}

void Editor::unset_color() const {
    if (has_colors() && !colorscheme_name_.empty()) {
        short color_pair = get_color_pair_index(current_color_pair_.foreground,
                                                current_color_pair_.background);
        attroff(COLOR_PAIR(color_pair));
    }
}

void Editor::print_message(const std::string &message) {
    set_color(ColorForeground::DEFAULT, ColorBackground::DEFAULT);
    mvprintw(LINES - 1, 0, "%s", message.c_str());
    clrtoeol();
    adjusted_move(cursor_position_.y, cursor_position_.x);
    unset_color();
}

void Editor::print_error(const std::string &error) {
    set_color(ColorForeground::COLOR1, ColorBackground::DEFAULT);
    mvprintw(LINES - 1, 0, "ERROR: %s", error.c_str());
    clrtoeol();
    adjusted_move(cursor_position_.y, cursor_position_.x);
    unset_color();
}

void Editor::run_command() {
    std::string::size_type space_delimiter = command_line_.find(" ");
    bool has_arg = space_delimiter != std::string::npos;
    std::string command = command_line_.substr(0, space_delimiter);
    std::string arg = has_arg ? command_line_.substr(space_delimiter + 1) : "";

    std::vector<Command> commands = get_command(command, arg);

    for (const Command &c : commands) {
        switch (c) {
            case Command::WRITE:
                save_file();
                break;
            case Command::QUIT:
                if (history_.has_unsaved_changes(buffer_.lines)) {
                    print_error("No write since last change");
                } else {
                    set_mode(Mode::EXIT);
                }
                break;
            case Command::FORCE_QUIT:
                set_mode(Mode::EXIT);
                break;
            case Command::PRINT_COLORSCHEME:
                if (arg.empty() && colorscheme_name_.empty()) {
                    print_error("No colorscheme detected");
                } else if (arg.empty()) {
                    print_message(colorscheme_name_);
                }
                break;
            case Command::SET_COLORSCHEME:
                if (colorschemes_.find(arg) != colorschemes_.end()) {
                    set_colorscheme(arg);
                } else {
                    print_error("Cannot find colorscheme '" + arg + "'");
                }
                break;
            case Command::JUMP_LINE:
                normal_jump_line(std::stoi(command) - 1);
                cursor_position_.x = buffer_.get_first_non_blank(
                    first_line_ + cursor_position_.y);
                last_column_ = cursor_position_.x;
                break;
            case Command::ERROR_INVALID_COMMAND:
                print_error("Not an editor command: " + command_line_);
                break;
            case Command::ERROR_TRAILING_CHARACTERS:
                print_error("Trailing characters");
                break;
        }
    }
}

void Editor::exit_command_mode() {
    cursor_position_.x = saved_position_.x;
    cursor_position_.y = saved_position_.y;
    move(LINES - 1, 0);
    clrtoeol();
    move(cursor_position_.y, cursor_position_.x);
}

void Editor::exit_insert_mode() {
    clear_command_line();
    int new_y = 0;
    int new_x = 0;
    getyx(stdscr, new_y, new_x);
    if (new_x - 1 >= line_number_width_ + 1) {
        --cursor_position_.x;
    }
    last_column_ = cursor_position_.x;
    adjusted_move(cursor_position_.y, cursor_position_.x);
}

void Editor::exit_normal_mode() { bind_count_.reset(); }

void Editor::exit_visual_mode() {
    clear_command_line();
    bind_count_.reset();
}

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
            case Mode::VISUAL:
                exit_visual_mode();
                break;
            default:
                break;
        }
        switch (new_mode) {
            case Mode::INSERT:
                print_message("-- INSERT --");
                break;
            case Mode::VISUAL:
                print_message("-- VISUAL --");
                visual_position_.x = cursor_position_.x;
                visual_position_.y = cursor_position_.y;
                break;
            default:
                break;
        }
        mode = new_mode;
    }
}
