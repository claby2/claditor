#ifndef CLADITOR_EDITOR_HPP
#define CLADITOR_EDITOR_HPP

#include <fstream>
#include <string>

#include "buffer.hpp"

enum class Mode { EXIT, NORMAL, INSERT, VISUAL };

class Editor {
   public:
    Mode mode;

    Editor();
    void set_file(std::string);
    void handle_input(int);
    void print_buffer();

   private:
    int x_;
    int y_;
    Buffer buffer_;
    std::fstream file_;
    std::string file_path_;

    void move_up();
    void move_right();
    void move_down();
    void move_left();
    void save_file();
};

#endif
