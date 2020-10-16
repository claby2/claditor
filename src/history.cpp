#include "history.hpp"

#include <string>
#include <vector>

History::History() = default;

bool History::has_unsaved_changes(const std::vector<std::string> &lines) const {
    return lines != lines_;
}

void History::set(const std::vector<std::string> &lines) { lines_ = lines; }
