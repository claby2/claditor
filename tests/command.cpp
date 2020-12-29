#include "command.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <vector>

bool command_equal(const Command &command1, const Command &command2) {
    // Compare if two commands are equal
    return command1.type == command2.type &&
           command1.content == command2.content && command1.arg == command2.arg;
}

bool commands_equal(const std::vector<Command> &commands1,
                    const std::vector<Command> &commands2) {
    // Compare if two vector of commands are equal
    if (commands1.size() != commands2.size()) {
        return false;
    }
    for (size_t i = 0; i < commands1.size(); ++i) {
        if (!command_equal(commands1[i], commands2[i])) {
            return false;
        }
    }
    return true;
}

TEST_CASE("Command error invalid command") {
    std::vector<Command> commands = get_command("invalid_command");
    std::vector<Command> expected{
        {CommandType::ERROR_INVALID_COMMAND, "invalid_command", ""}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command error trailing characters") {
    // Error when unexpected argument given to valid non-argument command
    std::vector<Command> commands = get_command("q argument");
    std::vector<Command> expected{
        {CommandType::ERROR_TRAILING_CHARACTERS, "q", "argument"}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command no command given") {
    std::vector<Command> commands = get_command("");
    REQUIRE(commands.empty());
}

TEST_CASE("Command jump line") {
    std::vector<Command> commands = get_command("123");
    std::vector<Command> expected{{CommandType::JUMP_LINE, "123", ""}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command valid with no argument") {
    std::vector<Command> commands = get_command("wq");
    std::vector<Command> expected{{CommandType::WRITE, "wq", ""},
                                  {CommandType::QUIT, "wq", ""}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command valid with argument") {
    std::vector<Command> commands = get_command("set tabs");
    std::vector<Command> expected{{CommandType::SET, "set", "tabs"}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command trailing and leading spaces") {
    std::vector<Command> commands = get_command("    set tabs    ");
    std::vector<Command> expected{{CommandType::SET, "set", "tabs"}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command multiple commands") {
    std::vector<Command> commands =
        get_command("set tabs | set number | 10 | wq");
    std::vector<Command> expected{{CommandType::SET, "set", "tabs"},
                                  {CommandType::SET, "set", "number"},
                                  {CommandType::JUMP_LINE, "10", ""},
                                  {CommandType::WRITE, "wq", ""},
                                  {CommandType::QUIT, "wq", ""}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}

TEST_CASE("Command multiple commands with quotes") {
    // '|' characters should be ignored when in quotes such as when echoing
    std::vector<Command> commands =
        get_command("echo \"hello | world\" | echo 'foo | bar'");
    std::vector<Command> expected{
        {CommandType::ECHO, "echo", "\"hello | world\""},
        {CommandType::ECHO, "echo", "'foo | bar'"}};
    bool equal = commands_equal(commands, expected);
    REQUIRE(equal);
}
