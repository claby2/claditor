#include "colorscheme_manager.hpp"

#include <dirent.h>

#include <fstream>
#include <sstream>

#include "color.hpp"
#include "interface.hpp"
#include "runtime.hpp"

void ColorschemeManager::fetch_colorschemes() {
    // Fetch colorschemes in runtime colors directory
    std::string home_directory = get_home_directory();
    std::string colors_directory = home_directory + "/.config/claditor/colors/";
    DIR *dir;
    dirent entry;
    dirent *result;
    if ((dir = opendir(colors_directory.c_str())) != 0) {
        while (readdir_r(dir, &entry, &result) == 0 && result != 0) {
            std::string file_name = entry.d_name;
            if (file_name.substr(file_name.find_last_of(".") + 1) == "clad") {
                // File has .clad file extension
                // Parse the file
                std::string colorscheme_name =
                    file_name.substr(0, file_name.find_last_of("."));
                std::ifstream file((colors_directory + file_name).c_str(),
                                   std::ios::in);
                std::stringstream file_stream;
                file_stream << file.rdbuf();
                file.close();
                Colorscheme colorscheme(file_stream);
                add_colorscheme(colorscheme_name, colorscheme);
            }
        }
        closedir(dir);
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
