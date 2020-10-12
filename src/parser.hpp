#ifndef CLADITOR_PARSER_HPP
#define CLADITOR_PARSER_HPP

#include <string>
#include <unordered_map>

class Parser {
   public:
    explicit Parser(std::string);
    std::string operator[](std::string);

   private:
    std::unordered_map<std::string, std::string> content_;
};
#endif
