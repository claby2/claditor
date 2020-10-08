#include <string>
#include <vector>

#include "buffer.hpp"

Buffer::Buffer() {}

void Buffer::push_back_line(std::string line) { lines.push_back(line); }

void Buffer::insert_line(std::string line, int row) {
    int index = row - 1;
    lines.insert(lines.begin() + row, line);
}
