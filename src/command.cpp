#include "command.hpp"

#include <string>
#include <unordered_map>
#include <vector>

bool is_valid_number(const std::string &str) {
    return str[0] != '0' &&
           str.find_first_not_of("0123456789") == std::string::npos;
}

bool validate_command(
    const std::unordered_map<std::string, std::vector<Command>> &commands_map,
    const std::string &command) {
    return commands_map.find(command) != commands_map.end();
}

std::vector<Command> get_command(const std::string &command,
                                 const std::string &arg) {
    std::vector<Command> commands_vector;
    if (command.empty()) {
        return commands_vector;
    }

    std::unordered_map<std::string, std::vector<Command>> COMMANDS = {
        {"q", {Command::QUIT}},
        {"q!", {Command::FORCE_QUIT}},
        {"w", {Command::WRITE}},
        {"wq", {Command::WRITE, Command::QUIT}},
        {"colo", {Command::PRINT_COLORSCHEME}},
        {"colorscheme", {Command::PRINT_COLORSCHEME}}};

    std::unordered_map<std::string, std::vector<Command>> ARG_COMMANDS = {
        {"colo", {Command::SET_COLORSCHEME}},
        {"colorscheme", {Command::SET_COLORSCHEME}},
        {"set", {Command::SET}}};

    bool has_arg = !arg.empty();

    // Check for special commands
    if (!has_arg && is_valid_number(command)) {
        commands_vector.push_back(Command::JUMP_LINE);
        return commands_vector;
    }

    bool is_valid_command = validate_command(COMMANDS, command);
    bool is_valid_arg_command =
        validate_command(ARG_COMMANDS, command) && has_arg;

    if (!is_valid_arg_command && !is_valid_command) {
        // Command is invalid
        commands_vector.push_back(Command::ERROR_INVALID_COMMAND);
        return commands_vector;
    }
    if (has_arg && !is_valid_arg_command && is_valid_command) {
        // Unexpected argument given
        commands_vector.push_back(Command::ERROR_TRAILING_CHARACTERS);
        return commands_vector;
    }

    if (has_arg) {
        commands_vector = ARG_COMMANDS[command];
    } else {
        commands_vector = COMMANDS[command];
    }
    return commands_vector;
}
