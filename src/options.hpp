#ifndef CLADITOR_OPTIONS_HPP
#define CLADITOR_OPTIONS_HPP

#include <string>
#include <unordered_map>

enum class OptionType { INT, STRING, BOOL };

class Options {
   public:
    Options();
    bool set_option(const std::string &);
    void set_options_from_config();
    int get_int_option(const std::string &);
    std::string get_string_option(const std::string &);
    bool get_bool_option(const std::string &);

   private:
    std::unordered_map<std::string, int> int_options_;
    std::unordered_map<std::string, std::string> string_options_;
    std::unordered_map<std::string, bool> bool_options_;

    bool is_valid_option(OptionType, const std::string &);
    static bool is_int_option_format(const std::string &);
};
#endif
