#ifndef CLADITOR_HISTORY_HPP
#define CLADITOR_HISTORY_HPP

#include <string>
#include <vector>

class History {
   public:
    History();
    bool has_unsaved_changes(const std::vector<std::string>&) const;
    void set_content(const std::vector<std::string>&);

   private:
    std::vector<std::string> lines_;
};
#endif
