#include <catch2/catch.hpp>
#include <string>
#include <vector>

#include "history.hpp"

TEST_CASE("History has unsaved changes") {
    History history;
    std::vector<std::string> original_lines = {"foo", "bar"};
    std::vector<std::string> new_lines = {"hello", "world"};
    history.set(original_lines);
    bool unsaved_changes = history.has_unsaved_changes(new_lines);
    REQUIRE(unsaved_changes);
}

TEST_CASE("History has no unsaved changes") {
    History history;
    std::vector<std::string> original_lines = {"foo", "bar"};
    std::vector<std::string> new_lines = {"foo", "bar"};
    history.set(original_lines);
    bool unsaved_changes = history.has_unsaved_changes(new_lines);
    REQUIRE_FALSE(unsaved_changes);
}
