#include "options.hpp"

#include <cctype>
#include <exception>
#include <stdexcept>
#include <string>

struct OptionError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

Options::Options()
    : int_options_{{"tabsize", 4}}, bool_options_{{"number", true}} {}

bool Options::set_option(const std::string &option) {
    if (is_int_option_format(option)) {
        // Given option is in the format of an int option
        std::string::size_type equal_delimiter = option.find('=');
        std::string option_name = option.substr(0, equal_delimiter);
        int option_value = std::stoi(option.substr(equal_delimiter + 1));
        if (is_valid_option(OptionType::INT, option_name)) {
            int_options_[option_name] = option_value;
        }
    } else if (is_valid_option(OptionType::BOOL, option)) {
        // Given option is a valid positive bool option
        bool_options_[option] = true;
    } else if (option.length() >= 2 && option.substr(0, 2) == "no" &&
               is_valid_option(OptionType::BOOL, option.substr(2))) {
        // Given option is a valid negative bool option
        bool_options_[option.substr(2)] = false;
    } else {
        // Given option is not valid
        return false;
    }
    return true;
}

int Options::operator[](const std::string &option) {
    if (is_valid_option(OptionType::INT, option)) {
        return int_options_[option];
    }
    if (is_valid_option(OptionType::BOOL, option)) {
        return static_cast<int>(bool_options_[option]);
    }
    throw OptionError("Invalid option requested");
}

bool Options::is_valid_option(OptionType option_type,
                              const std::string &option) {
    switch (option_type) {
        case OptionType::INT:
            return int_options_.find(option) != int_options_.end();
            break;
        case OptionType::BOOL:
            return bool_options_.find(option) != bool_options_.end();
            break;
    }
    return false;
}

bool Options::is_int_option_format(const std::string &option) {
    bool is_valid = false;
    bool found_equal = false;
    for (const char &c : option) {
        if (found_equal) {
            is_valid = static_cast<bool>(isdigit(c));
            if (!is_valid) {
                break;
            }
        } else if (c == '=') {
            found_equal = true;
        }
    }
    return found_equal && is_valid;
}
