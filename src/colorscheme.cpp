#include "colorscheme.hpp"

#include <unordered_map>

#include "color.hpp"
#include "parser.hpp"

Colorscheme::Colorscheme()
    : background(Color::black()),
      foreground(Color::white()),
      comment(Color::white()),
      accent(Color::white()),
      color1(Color::white()),
      color2(Color::white()),
      color3(Color::white()),
      color4(Color::white()),
      color5(Color::white()),
      color6(Color::white()) {}

Colorscheme::Colorscheme(const std::stringstream &file_stream) {
    // Set colorscheme from path
    Parser parser(FileType::COLOR, file_stream);
    std::unordered_map<std::string, std::string> color_content =
        parser.get_color_content();
    // Set colors of colorscheme
    background = get_color(color_content["background"], Color::black());
    foreground = get_color(color_content["foreground"], Color::white());
    comment = get_color(color_content["comment"], Color::white());
    accent = get_color(color_content["accent"], Color::white());
    color1 = get_color(color_content["color1"], Color::white());
    color2 = get_color(color_content["color2"], Color::white());
    color3 = get_color(color_content["color3"], Color::white());
    color4 = get_color(color_content["color4"], Color::white());
    color5 = get_color(color_content["color5"], Color::white());
    color6 = get_color(color_content["color6"], Color::white());
}
