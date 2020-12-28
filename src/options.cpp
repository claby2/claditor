#include "options.hpp"

#include <cctype>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "command.hpp"
#include "runtime.hpp"

struct OptionError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

Options::Options()
    : int_options_{{"tabsize", 4}},
      string_options_{{"colorscheme", ""}},
      bool_options_{{"number", true}, {"tabs", false}} {}

bool Options::set_option(const std::string &option) {
    std::string::size_type equal_delimiter = option.find('=');
    std::string option_name = option.substr(0, equal_delimiter);
    std::string option_value = option.substr(equal_delimiter + 1);
    if (equal_delimiter != std::string::npos &&
        is_valid_option(OptionType::INT, option_name) &&
        is_int_option_format(option)) {
        // Given option is a valid int option
        int_options_[option_name] = std::stoi(option_value);
    } else if (equal_delimiter != std::string::npos &&
               is_valid_option(OptionType::STRING, option_name)) {
        // Given option is a valid string option
        string_options_[option_name] = option_value;
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

void Options::set_options_from_config() {
    // Set options based on config content
    std::vector<std::string> config_content = get_runtime_config();
    for (const std::string &command : config_content) {
        std::vector<Command> commands = get_command(command);
        for (const Command &c : commands) {
            if (c.type == CommandType::SET) {
                set_option(c.arg);
            }
        }
    }
}

int Options::get_int_option(const std::string &option) {
    if (is_valid_option(OptionType::INT, option)) {
        return int_options_[option];
    }
    throw OptionError("Invalid int option requested");
}

std::string Options::get_string_option(const std::string &option) {
    if (is_valid_option(OptionType::STRING, option)) {
        return string_options_[option];
    }
    throw OptionError("Invalid string option requested");
}

bool Options::get_bool_option(const std::string &option) {
    if (is_valid_option(OptionType::BOOL, option)) {
        return bool_options_[option];
    }
    throw OptionError("Invalid bool option requested");
}

void Options::dump_config() {
    // Print int options
    for (const std::pair<std::string, int> p : int_options_) {
        std::cout << p.first << " " << p.second << '\n';
    }
    // Print string options
    for (const std::pair<std::string, std::string> p : string_options_) {
        std::cout << p.first << " \"" << p.second << "\"" << '\n';
    }
    // Print bool options
    for (const std::pair<std::string, bool> p : bool_options_) {
        std::cout << p.first << " " << (p.second ? "true" : "false") << '\n';
    }
}

bool Options::is_valid_option(OptionType option_type,
                              const std::string &option) {
    switch (option_type) {
        case OptionType::INT:
            return int_options_.find(option) != int_options_.end();
            break;
        case OptionType::STRING:
            return string_options_.find(option) != string_options_.end();
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
