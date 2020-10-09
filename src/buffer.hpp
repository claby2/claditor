#ifndef CLADITOR_BUFFER_HPP
#define CLADITOR_BUFFER_HPP

#include <string>
#include <vector>

class Buffer {
   public:
    std::vector<std::string> lines;

    Buffer();
    int get_line_length(int);
    int get_size() const;
    void push_back_line(std::string);
    void insert_line(std::string, int);
    void add_string_to_line(const std::string&, int);
    void erase(int, int, int);
    void insert_char(int, int, char, int);
    void remove_line(int);
};
#endif
