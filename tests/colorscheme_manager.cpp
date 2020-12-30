#include "colorscheme_manager.hpp"

#include <catch2/catch.hpp>
#include <string>

#include "color.hpp"

TEST_CASE("Colorscheme Manager set invalid colorscheme",
          "[colorscheme_manager]") {
    // Set colorscheme that does not exist
    ColorschemeManager colorscheme_manager;
    bool result = colorscheme_manager.set_colorscheme("invalid_colorscheme");
    CHECK(result == false);
    bool has_colorscheme = colorscheme_manager.has_colorscheme();
    CHECK(has_colorscheme == false);
}

TEST_CASE("Colorscheme manager set valid colorscheme",
          "[colorscheme_manager]") {
    std::string expected_colorscheme_name = "colorscheme_name";
    ColorschemeManager colorscheme_manager;

    // Initialize colorscheme
    Colorscheme colorscheme;
    colorscheme.background = {0, 0, 0};
    colorscheme.foreground = {0, 0, 0};
    colorscheme.comment = {0, 0, 0};
    colorscheme.accent = {0, 0, 0};
    colorscheme.color1 = {0, 0, 0};
    colorscheme.color2 = {0, 0, 0};
    colorscheme.color3 = {0, 0, 0};
    colorscheme.color4 = {0, 0, 0};
    colorscheme.color5 = {0, 0, 0};
    colorscheme.color6 = {0, 0, 0};

    colorscheme_manager.add_colorscheme(expected_colorscheme_name, colorscheme);
    bool result =
        colorscheme_manager.set_colorscheme(expected_colorscheme_name);
    CHECK(result == true);
    std::string colorscheme_name = colorscheme_manager.get_current_name();
    CHECK(colorscheme_name == expected_colorscheme_name);
    bool has_colorscheme = colorscheme_manager.has_colorscheme();
    CHECK(has_colorscheme == true);
}
