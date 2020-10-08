#ifndef CLADITOR_BUFFER_HPP
#define CLADITOR_BUFFER_HPP

#include <string>
#include <vector>

class Buffer {
   public:
    std::vector<std::string> lines;

    Buffer();
    void push_back_line(std::string);
    void insert_line(std::string, int);
};
#endif
