#include "editor.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Editor quit") {
    std::vector<int> inputs = {
        ':',
        'q',
        static_cast<int>(InputKey::ENTER),
    };

    std::stringstream file_stream;
    Editor editor("", file_stream);
    editor.set_interface_inputs(inputs);
    editor.start();
}
