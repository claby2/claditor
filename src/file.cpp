#include "file.hpp"

#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct FileError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

File::File(std::string new_file_path) : file_path_(std::move(new_file_path)) {}

std::vector<std::string> File::get_content() {
    std::vector<std::string> file_content;
    std::ifstream file;
    file.open(file_path_.c_str(), std::ios::in);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find('\t') != std::string::npos) {
            // Error if tabs are present
            throw FileError("Cannot open files with tabs");
        }
        file_content.push_back(line);
    }
    return file_content;
}

void File::write_content(const std::vector<std::string> &content) {
    if (!content.empty()) {
        std::ofstream file;
        file.open(file_path_.c_str(), std::ios::out);
        for (const std::string &line : content) {
            file << line << '\n';
        }
        file.close();
    }
}

std::string File::get_path() const { return file_path_; }
