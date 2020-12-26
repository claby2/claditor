#include "parser.hpp"

#include <algorithm>
#include <fstream>
#include <locale>
#include <string>
#include <unordered_map>
#include <vector>

Parser::Parser(FileType type, std::ifstream &file) : type_(type) {
    // Constructor is called if an ifstream is given rather than a stringstream
    if (file) {
        // Convert the file to stringstream
        file_stream_ << file.rdbuf();
        file.close();
    }
    set_type();
}

Parser::Parser(FileType type, const std::stringstream &file_stream)
    : type_(type) {
    file_stream_.str(file_stream.str());
    set_type();
}

std::unordered_map<std::string, std::string> Parser::get_color_content() const {
    return color_content_;
}

std::vector<std::string> Parser::get_config_content() const {
    return config_content_;
}

void Parser::set_type() {
    switch (type_) {
        case FileType::COLOR:
            parse_color_file();
            break;
        case FileType::CONFIG:
            parse_config_file();
            break;
        default:
            break;
    }
}

void Parser::parse_color_file() {
    std::string line;
    while (std::getline(file_stream_, line, '\n')) {
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
    while (std::getline(file_stream_, line, '\n')) {
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
