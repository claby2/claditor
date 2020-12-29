#ifndef CLADITOR_COMMAND_HPP
#define CLADITOR_COMMAND_HPP

#include <string>
#include <vector>

enum class CommandType {
    WRITE,
    QUIT,
    FORCE_QUIT,
    PRINT_COLORSCHEME,
    SET,
    ECHO,
    JUMP_LINE,

    // Error
    ERROR_INVALID_COMMAND,
    ERROR_TRAILING_CHARACTERS
};

struct Command {
    CommandType type;
    // content should store the string that instantiated the command without the
    // argument
    std::string content;
    std::string arg;

    Command(CommandType, const std::string &, const std::string &);
};

std::vector<Command> get_command(const std::string &);

#endif
