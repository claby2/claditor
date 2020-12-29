#include "command.hpp"

#include <algorithm>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

bool is_valid_number(const std::string &str) {
    return str[0] != '0' &&
           str.find_first_not_of("0123456789") == std::string::npos;
}

bool validate_command(
    const std::unordered_map<std::string, std::vector<CommandType>>
        &commands_map,
    const std::string &command) {
    return commands_map.find(command) != commands_map.end();
}

std::string trim(const std::string &str) {
    // Trim both leading and trailing white space
    if (str.empty()) {
        return str;
    }
    std::string::size_type first_scan = str.find_first_not_of(' ');
    std::string::size_type first =
        first_scan == std::string::npos ? str.length() : first_scan;
    std::string::size_type last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

Command::Command(CommandType type, const std::string &content,
                 const std::string &arg)
    : type(type), content(content), arg(arg) {}

std::vector<Command> parse_command(const std::string &str) {
    std::string command_line = trim(str);
    // Decompose command_line to command and argument constituents
    std::string::size_type space_delimiter = command_line.find(' ');
    std::string command = command_line.substr(0, space_delimiter);
    std::string arg = space_delimiter != std::string::npos
                          ? command_line.substr(space_delimiter + 1)
                          : "";
    std::vector<Command> commands_vector;
    if (command.empty()) {
        return commands_vector;
    }

    std::unordered_map<std::string, std::vector<CommandType>> COMMANDS = {
        {"q", {CommandType::QUIT}},
        {"q!", {CommandType::FORCE_QUIT}},
        {"w", {CommandType::WRITE}},
        {"wq", {CommandType::WRITE, CommandType::QUIT}},
        {"colo", {CommandType::PRINT_COLORSCHEME}},
        {"colorscheme", {CommandType::PRINT_COLORSCHEME}}};

    std::unordered_map<std::string, std::vector<CommandType>> ARG_COMMANDS = {
        {"set", {CommandType::SET}}, {"echo", {CommandType::ECHO}}};

    bool has_arg = !arg.empty();

    // Check for special commands
    if (!has_arg && is_valid_number(command)) {
        commands_vector.push_back({CommandType::JUMP_LINE, command, arg});
        return commands_vector;
    }

    bool is_valid_command = validate_command(COMMANDS, command);
    bool is_valid_arg_command =
        validate_command(ARG_COMMANDS, command) && has_arg;

    if (!is_valid_arg_command && !is_valid_command) {
        // Command is invalid
        commands_vector.push_back(
            {CommandType::ERROR_INVALID_COMMAND, command, arg});
        return commands_vector;
    }
    if (has_arg && !is_valid_arg_command && is_valid_command) {
        // Unexpected argument given
        commands_vector.push_back(
            {CommandType::ERROR_TRAILING_CHARACTERS, command, arg});
        return commands_vector;
    }

    // Determine vector to get command types depending on if argument is present
    std::vector<CommandType> types =
        has_arg ? ARG_COMMANDS[command] : COMMANDS[command];
    std::transform(types.begin(), types.end(),
                   std::back_inserter(commands_vector),
                   [command, arg](const CommandType &type) -> Command {
                       return {type, command, arg};
                   });

    return commands_vector;
}

std::vector<Command> get_command(const std::string &str) {
    // Break down str in case it contains multiple commands delimited by '|'
    // Additionally, '|' should be ignored when in quotes
    std::vector<std::string> command_strings;
    std::string current_command = "";
    std::stack<char> quotes;
    for (const char &c : str) {
        if (c == '|' && quotes.empty()) {
            // Found command
            command_strings.push_back(current_command);
            current_command = "";
        } else {
            current_command += c;
            if (c == '"' || c == '\'') {
                if (!quotes.empty() && quotes.top() == c) {
                    quotes.pop();
                } else {
                    quotes.push(c);
                }
            }
        }
    }
    if (!current_command.empty()) {
        // Current command is filled but has not been pushed back
        command_strings.push_back(current_command);
    }
    std::vector<Command> commands;
    for (const std::string &s : command_strings) {
        std::vector<Command> result = parse_command(s);
        commands.insert(commands.end(), result.begin(), result.end());
    }
    return commands;
}
