#include "command.hpp"

#include <algorithm>
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

Command::Command(CommandType type, const std::string &content,
                 const std::string &arg)
    : type(type), content(content), arg(arg) {}

std::vector<Command> get_command(const std::string &command_line) {
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
        {"set", {CommandType::SET}}};

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
