#include "runtime.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "parser.hpp"

std::string get_home_directory() {
    std::string home_directory;
#ifdef _WIN32
    const char* home_drive = getenv("HOMEDRIVE");
    const char* home_path = getenv("HOMEPATH");
    home_directory = strcat(home_drive, home_path);
#else
    home_directory = getenv("HOME");
#endif
    return home_directory;
}

std::vector<std::string> get_runtime_config() {
    const std::string CONFIGURATION_LOCATION =
        get_home_directory() + "/.config/claditor/cladrc";
    std::ifstream config;
    config.open(CONFIGURATION_LOCATION.c_str(), std::ios::in);
    std::vector<std::string> content;
    if (config) {
        Parser parser(FileType::CONFIG, config);
        content = parser.get_config_content();
    }
    config.close();
    return content;
}
