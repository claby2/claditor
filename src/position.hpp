#ifndef CLADITOR_POSITION_HPP
#define CLADITOR_POSITION_HPP

struct Position {
    Position();
    Position(int, int);

    bool operator==(const Position&) const;

    int y;
    int x;
};
#endif
