#include "parser.hpp"

#include <algorithm>
#include <fstream>
#include <locale>
#include <string>
#include <unordered_map>
#include <vector>

Parser::Parser(FileType type, const std::string &file_path) {
    file_.open(file_path.c_str(), std::ios::in);
    switch (type) {
        case FileType::COLOR:
            parse_color_file();
            break;
        case FileType::CONFIG:
            parse_config_file();
            break;
        default:
            break;
    }
    type_ = type;
}

std::string Parser::operator[](const std::string &key) const {
    if (type_ == FileType::COLOR) {
        auto it = color_content_.find(key);
        if (it != color_content_.end()) {
            // Key has been found in content
            return it->second;
        }
    }
    return "";
}

std::vector<std::string> Parser::get_config_content() const {
    return config_content_;
}

void Parser::parse_color_file() {
    std::string line;
    while (std::getline(file_, line)) {
        // Erase whitespace
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
                   line.end());
        if (!line.empty() && line.substr(0, 2) != "//") {
            std::string::size_type delimiter = 0;
            line = remove_comment(line);
            delimiter = line.find('=');
            if (delimiter != std::string::npos) {
                std::string key = line.substr(0, delimiter);
                std::string value = line.substr(delimiter + 1, line.length());
                color_content_[key] = value;
            }
        }
    }
}

void Parser::parse_config_file() {
    std::string line;
    while (std::getline(file_, line)) {
        if (!line.empty() && line.substr(0, 2) != "//") {
            line = remove_comment(line);
            // Remove leading whitespace
            std::string::size_type delimiter = 0;
            delimiter = line.find_last_not_of(" \t\n\v\f\r");
            if (delimiter != std::string::npos) {
                line.erase(delimiter + 1);
            }
            config_content_.push_back(line);
        }
    }
}

std::string Parser::remove_comment(const std::string &line) {
    // Remove comments delimiter by //
    std::string::size_type delimiter = 0;
    delimiter = line.find("//");
    return line.substr(0, delimiter);
}
