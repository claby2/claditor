#include "mode.hpp"

Mode::Mode(ModeType type) { type_ = type; }

ModeType Mode::get_type() const { return type_; }

void Mode::set_type(ModeType type) { type_ = type; }
