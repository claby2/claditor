#ifndef CLADITOR_EDITOR_HPP
#define CLADITOR_EDITOR_HPP

#include <fstream>
#include <string>
#include <unordered_map>

#include "bind_count.hpp"
#include "buffer.hpp"
#include "color.hpp"
#include "colorscheme_manager.hpp"
#include "file.hpp"
#include "history.hpp"
#include "interface.hpp"
#include "mode.hpp"
#include "options.hpp"
#include "position.hpp"

class Editor {
   public:
    explicit Editor(const std::string&);
    void start();

   private:
    Mode mode_;
    Position cursor_position_;
    Position saved_position_;
    Position visual_position_;
    int last_column_;
    int first_line_;
    int previous_first_line_;
    int current_line_;
    int visual_line_;
    int line_number_width_;
    int buffer_lines_;
    ColorPair current_color_pair_;
    bool zero_lines_;
    File file_;
    Options options_;
    ColorschemeManager colorscheme_manager_;
    std::string command_line_;
    BindCount bind_count_;
    Buffer buffer_;
    History history_;
    Interface interface_;

    void print_buffer();
    void print_command_line();
    void clear_command_line();
    void update();
    Position get_visual_start_position();
    Position get_visual_end_position();
    bool needs_visual_highlight(int, int);

    void normal_and_visual(int);
    bool normal_state(int);
    bool insert_state(int);
    void visual_state(int);
    bool visual_regular_state(int);
    bool visual_line_state(int);
    bool command_state(int);
    void state_enter(bool (Editor::*)(int));

    // Normal mode binds
    void normal_first_char();
    void normal_first_non_blank_char();
    void normal_delete();
    void normal_end_of_file();
    void normal_append_after_cursor();
    void normal_append_end_of_line();
    void normal_begin_new_line_below();
    void normal_begin_new_line_above();
    void normal_first_line();
    void normal_jump_line(int);
    void normal_center_line(int);
    void normal_delete_line();
    void normal_add_count(int);
    void normal_page_down();
    void normal_page_up();
    // Normal mode command states
    bool normal_command_g_state(int);
    bool normal_command_z_state(int);
    bool normal_command_d_state(int);
    bool normal_add_count_state(int);

    // Standard movement
    int get_adjusted_x();
    void adjusted_move(int, int) const;
    void move_up();
    void move_right();
    void move_down();
    void move_left();

    // Insert mode binds
    void insert_backspace();
    void insert_enter();
    void insert_tab();
    void insert_char(int);

    // Command mode binds
    void command_backspace();
    void command_enter();
    void command_char(int);

    // Visual mode binds
    void visual_delete_selection();

    void get_runtime_configuration();
    void get_colorschemes();
    void set_colorscheme(const std::string&);
    void set_color(ColorForeground, ColorBackground);
    void unset_color();
    void print_message(const std::string&);
    void print_error(const std::string&);
    void run_command();
    void exit_command_mode();
    void exit_insert_mode();
    void exit_normal_mode();
    void exit_visual_mode();
    void set_mode(ModeType);
};
#endif
