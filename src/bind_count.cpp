#include "bind_count.hpp"

BindCount::BindCount() : value_(0) {}

int BindCount::get_value() {
    int value = value_;
    // Reset value on access
    reset();
    // Return one if the value is empty
    return value == 0 ? 1 : value;
}

bool BindCount::empty() const { return value_ == 0; }

void BindCount::add_digit(int digit) {
    if (empty()) {
        value_ = digit;
    } else {
        value_ = (value_ * 10) + digit;
    }
}

void BindCount::reset() { value_ = 0; }
