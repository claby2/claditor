#ifndef CLADITOR_COLORSCHEME_MANAGER_HPP
#define CLADITOR_COLORSCHEME_MANAGER_HPP

#include <string>
#include <unordered_map>

#include "color.hpp"

class ColorschemeManager {
   public:
    void fetch_colorschemes();
    void add_colorscheme(const std::string&, const Colorscheme&);
    bool set_colorscheme(const std::string&);
    std::string get_current_name() const;
    bool has_colorscheme() const;

   private:
    std::string current_name_;
    std::unordered_map<std::string, Colorscheme> colorschemes_;
};
#endif
