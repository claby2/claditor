#include <catch2/catch.hpp>
#include <fstream>
#include <string>
#include <vector>

#include "file.hpp"

TEST_CASE("File get path") {
    File file("path");
    std::string file_path = file.get_path();
    REQUIRE(file_path == "path");
}

TEST_CASE("File get content") {
    File file("tests/test_files/file_text.txt");
    std::vector<std::string> file_content = file.get_content();
    std::vector<std::string> expected_file_content{"Test file", "", "content"};
    REQUIRE(file_content == expected_file_content);
}

TEST_CASE("File write empty content") {
    std::string file_path = "tests/test_files/this_file_should_not_exist.txt";
    File file(file_path);
    std::vector<std::string> empty_file_content;
    file.write_content(empty_file_content);
    bool file_exists = static_cast<bool>(std::ifstream(file_path));
    REQUIRE(file_exists == false);
}
