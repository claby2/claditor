#include "colorscheme_manager.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "color.hpp"
#include "interface.hpp"
#include "runtime.hpp"

void ColorschemeManager::fetch_colorschemes() {
    // Fetch colorschemes in runtime colors directory
    std::string home_directory = get_home_directory();
    std::string colors_directory = home_directory + "/.config/claditor/colors/";

    for (const std::filesystem::directory_entry &file :
         std::filesystem::directory_iterator(colors_directory)) {
        std::filesystem::path file_path = file.path();
        if (file_path.extension() == ".clad") {
            std::string filename =
                colors_directory + file_path.filename().string();
            std::ifstream file(filename.c_str(), std::ios::in);
            std::stringstream file_stream;
            file_stream << file.rdbuf();
            file.close();
            Colorscheme colorscheme(file_stream);
            add_colorscheme(file_path.stem(), colorscheme);
        }
    }
}

void ColorschemeManager::add_colorscheme(const std::string &colorscheme_name,
                                         const Colorscheme &colorscheme) {
    colorschemes_[colorscheme_name] = colorscheme;
}

bool ColorschemeManager::set_colorscheme(
    const std::string &new_colorscheme_name) {
    // Return true if given colorscheme name exists
    if (colorschemes_.find(new_colorscheme_name) != colorschemes_.end()) {
        Colorscheme colorscheme = colorschemes_[new_colorscheme_name];
        short current_color = 0;
        Interface::initialize_color(current_color, colorscheme.background);
        Interface::initialize_color(current_color, colorscheme.foreground);
        Interface::initialize_color(current_color, colorscheme.comment);
        Interface::initialize_color(current_color, colorscheme.accent);
        Interface::initialize_color(current_color, colorscheme.color1);
        Interface::initialize_color(current_color, colorscheme.color2);
        Interface::initialize_color(current_color, colorscheme.color3);
        Interface::initialize_color(current_color, colorscheme.color4);
        Interface::initialize_color(current_color, colorscheme.color5);
        Interface::initialize_color(current_color, colorscheme.color6);
        current_name_ = new_colorscheme_name;
        return true;
    }
    return false;
}

std::string ColorschemeManager::get_current_name() const {
    return current_name_;
}

bool ColorschemeManager::has_colorscheme() const {
    return !current_name_.empty();
}
