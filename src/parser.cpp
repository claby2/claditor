#include <algorithm>
#include <fstream>
#include <locale>
#include <string>
#include <unordered_map>

#include "parser.hpp"

Parser::Parser(std::string file_path) {
    std::ifstream file;
    file.open(file_path.c_str(), std::ios::in);
    std::string line;
    while (std::getline(file, line)) {
        // Erase whitespace
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
                   line.end());
        if (!line.empty() && line.substr(0, 2) != "//") {
            std::string::size_type delimiter;
            delimiter = line.find("//");
            if (delimiter != std::string::npos) {
                // Remove comments delimited by //
                line = line.substr(0, delimiter);
            }
            delimiter = line.find('=');
            if (delimiter != std::string::npos) {
                std::string key = line.substr(0, delimiter);
                std::string value = line.substr(delimiter + 1, line.length());
                content_[key] = value;
            }
        }
    }
}

std::string Parser::operator[](std::string key) {
    if (content_.find(key) != content_.end()) {
        // Key has been found in content
        return content_[key];
    }
    return "";
}
