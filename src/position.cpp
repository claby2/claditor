#include "position.hpp"

Position::Position() : y(0), x(0) {}

Position::Position(int new_y, int new_x) : y(new_y), x(new_x) {}

bool Position::operator==(const Position &position) const {
    return x == position.x && y == position.y;
}
