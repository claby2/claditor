#ifndef CLADITOR_PARSER_HPP
#define CLADITOR_PARSER_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

enum class FileType { COLOR, CONFIG };

class Parser {
   public:
    Parser(FileType, const std::string&);
    std::string operator[](const std::string&) const;
    std::vector<std::string> get_config_content() const;

   private:
    FileType type_;
    std::ifstream file_;
    std::unordered_map<std::string, std::string> color_content_;
    std::vector<std::string> config_content_;

    void parse_color_file();
    void parse_config_file();

    static std::string remove_comment(const std::string&);
};
#endif
