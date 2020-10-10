#ifndef CLADITOR_EDITOR_HPP
#define CLADITOR_EDITOR_HPP

#include <fstream>
#include <string>

#include "buffer.hpp"
#include "history.hpp"

enum class Mode { EXIT, NORMAL, INSERT, VISUAL, COMMAND };

class Editor {
   public:
    Mode mode;

    Editor();
    void set_file(std::string);
    void main();

   private:
    int x_;
    int y_;
    int saved_x_;
    int saved_y_;
    int last_column_;
    int first_line_;
    int current_line_;
    int line_number_width_;
    bool file_started_empty_;
    std::string command_line_;
    std::string normal_bind_buffer_;
    Buffer buffer_;
    History history_;
    std::string file_path_;

    void print_buffer();
    void print_command_line();
    void update();

    bool normal_state(int);
    bool insert_state(int);
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
    void normal_delete_line();
    // Normal mode command states
    bool normal_command_g_state(int);
    bool normal_command_d_state(int);

    // Standard movement
    int get_adjusted_x();
    void adjusted_move(int, int);
    void move_up();
    void move_right();
    void move_down();
    void move_left();

    // Insert mode binds
    void insert_backspace();
    void insert_enter();
    void insert_char(int);

    // Command mode binds
    void command_backspace();
    void command_enter();
    void command_char(int);

    void save_file();
    void print_message(const std::string&);
    void print_error(const std::string&);
    void parse_command();
    void exit_command_mode();
    void exit_insert_mode();
    void exit_normal_mode();
    void set_mode(Mode);
};
#endif
