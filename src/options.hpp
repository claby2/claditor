#ifndef CLADITOR_OPTIONS_HPP
#define CLADITOR_OPTIONS_HPP

#include <string>
#include <unordered_map>

enum class OptionType { BOOL, INT };

class Options {
   public:
    Options();
    bool set_option(const std::string&);
    int operator[](const std::string&);

   private:
    std::unordered_map<std::string, int> int_options_;
    std::unordered_map<std::string, bool> bool_options_;

    bool is_valid_option(OptionType, const std::string&);
    static bool is_int_option_format(const std::string&);
};
#endif
