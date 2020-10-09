#ifndef CLADITOR_EDITOR_HPP
#define CLADITOR_EDITOR_HPP

#include <fstream>
#include <string>

#include "buffer.hpp"

enum class Mode { EXIT, NORMAL, INSERT, VISUAL, COMMAND };

class Editor {
   public:
    Mode mode;

    Editor();
    void set_file(std::string);
    void handle_input(int);
    void print_buffer();
    void print_command_line() const;

   private:
    int x_;
    int y_;
    int saved_x_;
    int saved_y_;
    int last_column_;
    bool file_started_empty;
    std::string command_line_;
    Buffer buffer_;
    std::string file_path_;

    int get_adjusted_x();
    void move_up();
    void move_right();
    void move_down();
    void move_left();
    void save_file();
    void exit_command_mode();
    void exit_insert_mode();
    void parse_command();
};

#endif
