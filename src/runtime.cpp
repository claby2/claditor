#include "runtime.hpp"

#include <cstdlib>
#include <cstring>
#include <string>

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
