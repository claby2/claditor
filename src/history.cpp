#include <string>
#include <vector>

#include "history.hpp"

History::History() {}

bool History::has_unsaved_changes(const std::vector<std::string> &lines) const {
    return lines != lines_;
}

void History::set(const std::vector<std::string> &lines) { lines_ = lines; }
