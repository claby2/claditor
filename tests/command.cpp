#include "command.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <vector>

TEST_CASE("Command error invalid command") {
    std::string command = "invalid_command";
    std::string arg = "";
    std::vector<Command> commands = get_command(command, arg);
    std::vector<Command> expected_commands{Command::ERROR_INVALID_COMMAND};
    REQUIRE(commands == expected_commands);
}

TEST_CASE("Command error trailing characters") {
    // Error when unexpected argument given to valid non-argument command
    std::string command = "q";
    std::string arg = "argument";
    std::vector<Command> commands = get_command(command, arg);
    std::vector<Command> expected_commands{Command::ERROR_TRAILING_CHARACTERS};
    REQUIRE(commands == expected_commands);
}

TEST_CASE("Command no command given") {
    std::string command = "";
    std::string arg = "";
    std::vector<Command> commands = get_command(command, arg);
    REQUIRE(commands.empty());
}

TEST_CASE("Command jump line") {
    std::string command = "123";
    std::string arg = "";
    std::vector<Command> commands = get_command(command, arg);
    std::vector<Command> expected_commands{Command::JUMP_LINE};
    REQUIRE(commands == expected_commands);
}

TEST_CASE("Command valid with no argument") {
    std::string command = "wq";
    std::string arg = "";
    std::vector<Command> commands = get_command(command, arg);
    std::vector<Command> expected_commands{Command::WRITE, Command::QUIT};
    REQUIRE(commands == expected_commands);
}

TEST_CASE("Command valid with argument") {
    std::string command = "colorscheme";
    std::string arg = "default";
    std::vector<Command> commands = get_command(command, arg);
    std::vector<Command> expected_commands{Command::SET_COLORSCHEME};
    REQUIRE(commands == expected_commands);
}
