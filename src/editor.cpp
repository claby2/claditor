#include "editor.hpp"

#include <ncurses.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

#include "bind_count.hpp"
#include "buffer.hpp"
#include "color.hpp"
#include "command.hpp"
#include "interface.hpp"
#include "options.hpp"
#include "parser.hpp"
#include "position.hpp"
#include "runtime.hpp"

// Return the equivalent input code when input and ctrl keys are held together
#define ctrl(input) ((input)&0x1f)

enum class InputKey : int { TAB = 9, ENTER = 10, ESCAPE = 27, BACKSPACE = 127 };

// Backspace cross-platform compatibility
#define IS_BACKSPACE                            \
    case static_cast<int>(InputKey::BACKSPACE): \
    case KEY_BACKSPACE:                         \
    case KEY_DC

Editor::Editor(const std::string &file_path)
    : mode_(ModeType::NORMAL),
      cursor_position_(0, 0),
      saved_position_(0, 0),
      visual_position_(0, 0),
      last_column_(0),
      first_line_(0),
      previous_first_line_(0),
      current_line_(0),
      visual_line_(0),
      line_number_width_(0),
      buffer_lines_(0),
      current_color_pair_{ColorForeground::DEFAULT, ColorBackground::DEFAULT},
      zero_lines_(false),
      file_(file_path) {
    buffer_.lines = file_.get_content();
    if (buffer_.get_size() == 0) {
        // Add empty line to prevent segmentation fault
        buffer_.push_back_line("");
        zero_lines_ = true;
    }
    history_.set_content(buffer_.lines);
}

void Editor::start() {
    Interface::refresh();
    interface_.update();
    options_.set_options_from_config();
    colorscheme_manager_.fetch_colorschemes();
    colorscheme_manager_.set_colorscheme(
        options_.get_string_option("colorscheme"));
    state_enter(&Editor::normal_state);
}

void Editor::print_buffer() {
    if (first_line_ >= buffer_.get_size()) {
        // Adjusted first line
        first_line_ = buffer_.get_size() - 1;
        cursor_position_.y = 0;
    }
    bool has_scroll = previous_first_line_ != first_line_;
    if (has_scroll) {
        Interface::cursor_set(0);
    }
    set_color(ColorForeground::DEFAULT, ColorBackground::DEFAULT);
    ColorPair default_color_pair = current_color_pair_;
    for (int i = 0; i < buffer_lines_; ++i) {
        if (first_line_ + i >= buffer_.get_size()) {
            Interface::move_cursor(i, 0);
        } else {
            std::string line = buffer_.lines[first_line_ + i];
            if (options_.get_bool_option("number")) {
                std::string line_number = std::to_string(first_line_ + i + 1);
                std::string line_number_content =
                    std::string(line_number_width_ - line_number.length(),
                                ' ') +
                    line_number;
                // Print line number
                Interface::mv_print(i, 0, line_number_content + ' ');
            }
            // Print characters one by one
            for (size_t j = 0; j < line.length(); ++j) {
                bool accent = needs_visual_highlight(i, j) || line[j] == '\t';
                if (accent) {
                    unset_color();
                    set_color(ColorForeground::DEFAULT,
                              ColorBackground::ACCENT);
                }
                Interface::mv_print_ch(
                    i, static_cast<int>(line_number_width_ + 1 + j), line[j]);
                if (accent) {
                    unset_color();
                    set_color(default_color_pair.foreground,
                              default_color_pair.background);
                }
            }
        }
        Interface::clear_to_eol();
        if (has_scroll) {
            Interface::refresh();
        }
    }
    unset_color();
    adjusted_move(cursor_position_.y, cursor_position_.x);
    if (has_scroll) {
        Interface::cursor_set(1);
    }
    previous_first_line_ = first_line_;
}

void Editor::print_command_line() {
    set_color(ColorForeground::DEFAULT, ColorBackground::DEFAULT);
    if (mode_.get_type() == ModeType::COMMAND) {
        Interface::mv_print(buffer_lines_, 0, ':' + command_line_);
        Interface::clear_to_eol();
    }
    unset_color();
}

void Editor::clear_command_line() {
    command_line_ = "";
    Interface::move_cursor(buffer_lines_, 0);
    Interface::clear_to_eol();
    adjusted_move(cursor_position_.y, cursor_position_.x);
}

void Editor::update() {
    // Number of screen lines used for command-line
    const int COMMAND_HEIGHT = 1;
    int initial_interface_lines = interface_.lines;
    interface_.update();
    buffer_lines_ = interface_.lines - COMMAND_HEIGHT;
    // Handle resizing
    if (initial_interface_lines != interface_.lines) {
        normal_center_line(current_line_);
    }
    if (zero_lines_ &&
        (buffer_.get_size() != 1 || buffer_.get_line_length(0) > 0)) {
        zero_lines_ = false;
    }
    current_line_ = first_line_ + cursor_position_.y;
    line_number_width_ =
        (options_.get_bool_option("number") == 1)
            ? static_cast<int>(std::to_string(buffer_.get_size() + 1).length() +
                               1)
            : -1;
    Interface::refresh();
}

Position Editor::get_visual_start_position() {
    // Return position of the start of visual selection
    Position start = visual_position_;
    if (current_line_ == visual_line_) {
        start = {cursor_position_.y,
                 std::min(cursor_position_.x, visual_position_.x)};
    } else if (current_line_ < visual_line_) {
        start = cursor_position_;
    } else {
        start.y = visual_line_ - first_line_;
    }
    if (mode_.get_type() == ModeType::VISUAL_LINE) {
        start.x = 0;
    }
    return start;
}

Position Editor::get_visual_end_position() {
    // Return position of the end of visual selection
    Position end = cursor_position_;
    if (current_line_ == visual_line_) {
        end = {cursor_position_.y,
               std::max(cursor_position_.x, visual_position_.x)};
    } else if (current_line_ < visual_line_) {
        end = {visual_line_ - first_line_, visual_position_.x};
    }
    if (mode_.get_type() == ModeType::VISUAL_LINE) {
        end.x = buffer_.get_line_length(end.y + first_line_) - 1;
    }
    return end;
}

bool Editor::needs_visual_highlight(int y, int x) {
    if ((mode_.get_type() != ModeType::VISUAL &&
         mode_.get_type() != ModeType::VISUAL_LINE) ||
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
            set_mode(ModeType::COMMAND);
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
        case ctrl('f'):
            normal_page_down();
            break;
        case ctrl('b'):
            normal_page_up();
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
            set_mode(ModeType::INSERT);
            state_enter(&Editor::insert_state);
            break;
        case 'v':
            set_mode(ModeType::VISUAL);
            state_enter(&Editor::visual_regular_state);
            break;
        case 'V':
            set_mode(ModeType::VISUAL_LINE);
            state_enter(&Editor::visual_line_state);
            break;
        case 'o':
            normal_begin_new_line_below();
            break;
        case 'O':
            normal_begin_new_line_above();
            break;
        case 'z':
            state_enter(&Editor::normal_command_z_state);
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
            set_mode(ModeType::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        IS_BACKSPACE:
            insert_backspace();
            break;
        case static_cast<int>(InputKey::ENTER):
            insert_enter();
            break;
        case static_cast<int>(InputKey::TAB):
            insert_tab();
            break;
        default:
            insert_char(input);
            break;
    }
    return true;
}

void Editor::visual_state(int input) {
    // Visual binds for all visual mode variations
    switch (input) {
        case static_cast<int>(InputKey::ESCAPE):
            set_mode(ModeType::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        case 'd':
            visual_delete_selection();
            set_mode(ModeType::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        default:
            normal_and_visual(input);
            break;
    }
}

bool Editor::visual_regular_state(int input) {
    switch (input) {
        case 'v':
            set_mode(ModeType::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        case 'V':
            set_mode(ModeType::VISUAL_LINE);
            state_enter(&Editor::visual_line_state);
            break;
        default:
            visual_state(input);
            break;
    }
    return true;
}

bool Editor::visual_line_state(int input) {
    switch (input) {
        case 'v':
            set_mode(ModeType::VISUAL);
            state_enter(&Editor::visual_regular_state);
            break;
        case 'V':
            set_mode(ModeType::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        default:
            visual_state(input);
            break;
    }
    return true;
}

bool Editor::command_state(int input) {
    switch (input) {
        case static_cast<int>(InputKey::ESCAPE):
            set_mode(ModeType::NORMAL);
            state_enter(&Editor::normal_state);
            break;
        IS_BACKSPACE:
            command_backspace();
            break;
        case static_cast<int>(InputKey::ENTER):
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
        input = Interface::get_input();
    } while ((this->*state_callback)(input) &&
             mode_.get_type() != ModeType::EXIT);
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
    if (buffer_.get_size() > buffer_lines_) {
        first_line_ = last_line - buffer_lines_ + 1;
        cursor_position_.y = buffer_lines_ - 1;
    } else {
        first_line_ = 0;
        cursor_position_.y = last_line;
    }
    cursor_position_.x = buffer_.get_first_non_blank(last_line);
    last_column_ = cursor_position_.x;
}

void Editor::normal_append_after_cursor() {
    ++cursor_position_.x;
    set_mode(ModeType::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_append_end_of_line() {
    cursor_position_.x = buffer_.get_line_length(current_line_);
    set_mode(ModeType::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_begin_new_line_below() {
    buffer_.insert_line("", current_line_ + 1);
    cursor_position_.x = 0;
    if (cursor_position_.y >= buffer_lines_ - 1) {
        ++first_line_;
    } else {
        ++cursor_position_.y;
    }
    set_mode(ModeType::INSERT);
    state_enter(&Editor::insert_state);
}

void Editor::normal_begin_new_line_above() {
    buffer_.insert_line("", current_line_);
    cursor_position_.x = 0;
    set_mode(ModeType::INSERT);
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
    if (line >= first_line_ && line < first_line_ + buffer_lines_) {
        // Target line is already visible
        // There is no need to change first_line_
        cursor_position_.y = line - first_line_;
    } else if (std::abs(line -
                        (first_line_ + std::floor(interface_.lines / 2))) >
               interface_.lines) {
        // Make cursor_position_.y middle point of screen
        first_line_ = std::max(
            0, std::min(
                   buffer_.get_size() - buffer_lines_,
                   static_cast<int>(line - std::floor(interface_.lines / 2))));
        cursor_position_.y = line - first_line_;
    } else if (line < current_line_) {
        first_line_ = line;
        cursor_position_.y = 0;
    } else {
        first_line_ = line - buffer_lines_ + 1;
        cursor_position_.y = buffer_lines_ - 1;
    }
}

void Editor::normal_center_line(int line) {
    // Ensure that line exists in buffer
    line = std::max(0, std::min(buffer_.get_size() - 1, line));
    first_line_ =
        std::max(0, static_cast<int>(line - std::floor((buffer_lines_) / 2)));
    cursor_position_.y = line - first_line_;
}

void Editor::normal_delete_line() {
    if (buffer_.get_size() > 1) {
        buffer_.remove_line(current_line_);
        cursor_position_.x = buffer_.get_first_non_blank(current_line_);
        if (current_line_ >= buffer_.get_size()) {
            --cursor_position_.y;
        }
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

void Editor::normal_page_down() {
    // Bottom line on screen becomes the first line
    first_line_ =
        std::min(first_line_ + buffer_lines_ - 1, buffer_.get_size() - 1);
    if (first_line_ + cursor_position_.y >= buffer_.get_size()) {
        normal_end_of_file();
    }
}

void Editor::normal_page_up() {
    // First line on screen bcomes the bottom line
    first_line_ = std::max(first_line_ - buffer_lines_ + 1, 0);
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

bool Editor::normal_command_z_state(int input) {
    switch (input) {
        case 'z':  // Bind: zz
            if (bind_count_.empty()) {
                normal_center_line(current_line_);
            } else {
                normal_center_line(bind_count_.get_value() - 1);
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

void Editor::adjusted_move(int y, int x) const {
    // Move cursor with line number width offset
    Interface::move_cursor(y, line_number_width_ + x + 1);
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
        // In normal mode, the cursor should not be ahead of the end of the line
        if (cursor_position_.x + line_number_width_ + 1 <= interface_.columns &&
            cursor_position_.x < buffer_.get_line_length(current_line_) &&
            !(mode_.get_type() == ModeType::NORMAL &&
              cursor_position_.x + 1 >=
                  buffer_.get_line_length(current_line_))) {
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
        if (cursor_position_.y + 1 < buffer_lines_ &&
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
    if (static_cast<std::string::size_type>(cursor_position_.x) <
        buffer_.lines[current_line_].length()) {
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
    if (cursor_position_.y >= buffer_lines_ - 1) {
        // If cursor is at the bottom of the screen, only increase first line
        ++first_line_;
    } else {
        ++cursor_position_.y;
    }
}

void Editor::insert_tab() {
    if (options_.get_bool_option("tabs")) {
        buffer_.insert_char(cursor_position_.x, 1, '\t', current_line_);
        ++cursor_position_.x;
    } else {
        int tabsize = options_.get_int_option("tabsize");
        buffer_.insert_char(cursor_position_.x, tabsize, ' ', current_line_);
        cursor_position_.x += tabsize;
    }
}

void Editor::insert_char(int input) {
    buffer_.insert_char(cursor_position_.x, 1, static_cast<char>(input),
                        current_line_);
    ++cursor_position_.x;
}

void Editor::command_backspace() {
    if (command_line_.empty()) {
        set_mode(ModeType::NORMAL);
        state_enter(&Editor::insert_state);
    } else {
        command_line_.pop_back();
    }
}

void Editor::command_enter() {
    set_mode(ModeType::NORMAL);
    run_command();
    if (mode_.get_type() != ModeType::EXIT) {
        state_enter(&Editor::normal_state);
    }
}

void Editor::command_char(int input) {
    command_line_ += static_cast<char>(input);
}

void Editor::visual_delete_selection() {
    Position start = get_visual_start_position();
    Position end = get_visual_end_position();
    // Y positions may not equal to the line it represents
    if (cursor_position_ == start) {
        start.y = current_line_;
        end.y = visual_line_;
    } else {
        start.y = visual_line_;
        end.y = current_line_;
    }
    if (start.y == end.y) {
        buffer_.erase(start.x, end.x - start.x + 1, start.y);
    } else {
        // Erase selected content
        buffer_.erase(start.x, buffer_.get_line_length(start.y), start.y);
        for (int i = start.y + 1; i < end.y; ++i) {
            buffer_.set_line("", i);
        }
        buffer_.erase(0, end.x + 1, end.y);

        // Concatenate end line and start line
        if (buffer_.get_line_length(end.y) == 0) {
            // If end line is currently empty, concatenate with the line after
            // Ensure that it doesn't exceed maximum lines
            end.y = std::min(buffer_.get_size() - 1, end.y + 1);
        }
        buffer_.add_string_to_line(buffer_.lines[end.y], start.y);
        buffer_.set_line("", end.y);

        // Remove empty lines
        for (int i = end.y; i >= start.y; --i) {
            if (buffer_.get_line_length(i) == 0) {
                buffer_.remove_line(i);
            }
        }

        // Check if entire buffer was deleted
        if (buffer_.get_size() == 0) {
            zero_lines_ = true;
            buffer_.push_back_line("");
        }
    }
    // Set cursor position to start of selection with adjustment if needed
    cursor_position_ = {
        std::min(buffer_.get_size() - 1, start.y) - first_line_,
        std::max(0, std::min(buffer_.get_line_length(start.y) - 1, start.x))};
}

void Editor::set_color(ColorForeground foreground, ColorBackground background) {
    if (Interface::has_color_capability() &&
        colorscheme_manager_.has_colorscheme()) {
        short color_pair = get_color_pair_index(foreground, background);
        Interface::attribute_on(color_pair);
        current_color_pair_ = {foreground, background};
    }
}

void Editor::unset_color() {
    if (Interface::has_color_capability() &&
        colorscheme_manager_.has_colorscheme()) {
        short color_pair = get_color_pair_index(current_color_pair_.foreground,
                                                current_color_pair_.background);
        Interface::attribute_off(color_pair);
    }
}

void Editor::print_message(const std::string &message) {
    set_color(ColorForeground::DEFAULT, ColorBackground::DEFAULT);
    Interface::mv_print(buffer_lines_, 0, message);
    Interface::clear_to_eol();
    adjusted_move(cursor_position_.y, cursor_position_.x);
    unset_color();
}

void Editor::print_error(const std::string &error) {
    set_color(ColorForeground::COLOR1, ColorBackground::DEFAULT);
    Interface::mv_print(buffer_lines_, 0, "ERROR: " + error);
    Interface::clear_to_eol();
    adjusted_move(cursor_position_.y, cursor_position_.x);
    unset_color();
}

void Editor::run_command() {
    std::vector<Command> commands = get_command(command_line_);

    for (const Command &c : commands) {
        switch (c.type) {
            case CommandType::WRITE:
                file_.write_content(buffer_.lines);
                history_.set_content(buffer_.lines);
                print_message("\"" + file_.get_path() + "\" written");
                break;
            case CommandType::QUIT:
                if (history_.has_unsaved_changes(buffer_.lines)) {
                    print_error("No write since last change");
                } else {
                    set_mode(ModeType::EXIT);
                }
                break;
            case CommandType::FORCE_QUIT:
                set_mode(ModeType::EXIT);
                break;
            case CommandType::PRINT_COLORSCHEME:
                if (c.arg.empty() && !colorscheme_manager_.has_colorscheme()) {
                    print_error("No colorscheme detected");
                } else if (c.arg.empty()) {
                    print_message(colorscheme_manager_.get_current_name());
                }
                break;
            case CommandType::SET: {
                std::string initial_colorscheme =
                    options_.get_string_option("colorscheme");
                if (!options_.set_option(c.arg)) {
                    print_error("Unknown option: " + c.arg);
                }
                // Check if colorscheme has changed, set new colorscheme changed
                std::string new_colorscheme =
                    options_.get_string_option("colorscheme");
                if (initial_colorscheme != new_colorscheme &&
                    !colorscheme_manager_.set_colorscheme(new_colorscheme)) {
                    print_error("Cannot find colorscheme '" + c.arg + "'");
                }
            } break;
            case CommandType::JUMP_LINE:
                normal_jump_line(std::stoi(c.content) - 1);
                cursor_position_.x = buffer_.get_first_non_blank(
                    first_line_ + cursor_position_.y);
                last_column_ = cursor_position_.x;
                break;
            case CommandType::ERROR_INVALID_COMMAND:
                print_error("Not an editor command: " + command_line_);
                break;
            case CommandType::ERROR_TRAILING_CHARACTERS:
                print_error("Trailing characters");
                break;
        }
    }
}

void Editor::exit_command_mode() {
    cursor_position_.x = saved_position_.x;
    cursor_position_.y = saved_position_.y;
    Interface::move_cursor(buffer_lines_, 0);
    Interface::clear_to_eol();
    Interface::move_cursor(cursor_position_.y, cursor_position_.x);
}

void Editor::exit_insert_mode() {
    clear_command_line();
    int new_x = Interface::get_current_x();
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

void Editor::set_mode(ModeType new_type) {
    if (mode_.get_type() != new_type) {
        switch (mode_.get_type()) {
            case ModeType::INSERT:
                exit_insert_mode();
                break;
            case ModeType::COMMAND:
                exit_command_mode();
                break;
            case ModeType::NORMAL:
                exit_normal_mode();
                break;
            case ModeType::VISUAL:
            case ModeType::VISUAL_LINE:
                exit_visual_mode();
                break;
            default:
                break;
        }
        switch (new_type) {
            case ModeType::INSERT:
                print_message("-- INSERT --");
                break;
            case ModeType::VISUAL:
                print_message("-- VISUAL --");
                visual_position_ = cursor_position_;
                visual_line_ = current_line_;
                break;
            case ModeType::VISUAL_LINE:
                print_message("-- V-LINE --");
                visual_position_ = cursor_position_;
                visual_line_ = current_line_;
                break;
            default:
                break;
        }
        mode_.set_type(new_type);
    }
}
