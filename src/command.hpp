#ifndef CLADITOR_COMMAND_HPP
#define CLADITOR_COMMAND_HPP

#include <string>
#include <vector>

enum class Command {
    WRITE,
    QUIT,
    FORCE_QUIT,
    PRINT_COLORSCHEME,
    SET_COLORSCHEME,
    SET,
    JUMP_LINE,

    // Error
    ERROR_INVALID_COMMAND,
    ERROR_TRAILING_CHARACTERS
};

std::vector<Command> get_command(const std::string&, const std::string&);

#endif
