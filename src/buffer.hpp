#ifndef CLADITOR_BUFFER_HPP
#define CLADITOR_BUFFER_HPP

#include <string>
#include <vector>

#include "position.hpp"

class Buffer {
   public:
    Position position;
    std::vector<std::string> lines;

    Buffer();
    int get_line_length(int);
    int get_size() const;
    int get_first_non_blank(int);
    void set_line(const std::string&, int);
    void push_back_line(const std::string&);
    void insert_line(const std::string&, int);
    void add_string_to_line(const std::string&, int);
    void erase(int, int, int);
    void insert_char(int, int, char, int);
    void remove_line(int);
};
#endif
