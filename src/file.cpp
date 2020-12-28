#include "file.hpp"

#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct FileError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

File::File(const std::string &file_path, const std::stringstream &file_stream)
    : file_path_(file_path) {
    file_stream_.str(file_stream.str());
}

std::vector<std::string> File::get_content() {
    std::string line;
    std::vector<std::string> file_content;
    while (getline(file_stream_, line, '\n')) {
        file_content.push_back(line);
    }
    return file_content;
}

void File::write_content(const std::vector<std::string> &content) {
#ifndef UNIT_TEST
    if (!content.empty()) {
        std::ofstream file;
        file.open(file_path_.c_str(), std::ios::out);
        for (const std::string &line : content) {
            file << line << '\n';
        }
        file.close();
    }
#endif
}

std::string File::get_path() const { return file_path_; }
