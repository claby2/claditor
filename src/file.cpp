#include "file.hpp"

#include <fstream>
#include <string>
#include <utility>
#include <vector>

File::File(std::string new_file_path) : file_path_(std::move(new_file_path)) {}

std::vector<std::string> File::get_content() {
    std::vector<std::string> file_content;
    std::ifstream file;
    file.open(file_path_.c_str(), std::ios::in);
    std::string line;
    while (std::getline(file, line)) {
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
