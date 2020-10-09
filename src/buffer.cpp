#include <string>
#include <vector>

#include "buffer.hpp"

Buffer::Buffer() {}

int Buffer::get_line_length(int row) {
    return static_cast<int>(lines[row].length());
}

int Buffer::get_size() const { return static_cast<int>(lines.size()); }

void Buffer::push_back_line(std::string line) { lines.push_back(line); }

void Buffer::insert_line(std::string line, int row) {
    lines.insert(lines.begin() + row, line);
}

void Buffer::add_string_to_line(const std::string &line, int row) {
    lines[row] += line;
}

void Buffer::erase(int position, int length, int row) {
    lines[row].erase(position, length);
}

void Buffer::insert_char(int position, int n, char character, int row) {
    // Fill line at row with character n times from a given position
    lines[row].insert(position, n, character);
}

void Buffer::remove_line(int row) { lines.erase(lines.begin() + row); }
