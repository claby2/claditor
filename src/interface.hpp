#ifndef CLADITOR_INTERFACE_HPP
#define CLADITOR_INTERFACE_HPP

#include <string>
#include <vector>

struct Color;

class Interface {
   public:
    int lines;    // LINES
    int columns;  // COLS

    Interface();

    void update();

    static int refresh();                                // refresh
    static int cursor_set(int);                          // curs_set
    static int move_cursor(int, int);                    // move
    static int mv_print(int, int, const std::string &);  // mvprintw
    static int mv_print_ch(int, int, char);              // mvaddch
    static int clear_to_eol();                           // clrtoeol
    static int attribute_on(short);                      // attron
    static int attribute_off(short);                     // attroff
    static int get_current_y();                          // getcury
    static int get_current_x();                          // getcurx
    int get_input();                                     // getch
    static int initialize_color(short &, Color);         // init_color
    static bool has_color_capability();                  // has_colors

#ifdef UNIT_TEST
    void set_inputs(const std::vector<int> &);
    void set_dimensions(int, int);

   private:
    // Mock input for unit test by storing sequential inputs in vector
    std::vector<int> inputs_;
    int current_input_ = 0;
#endif
};
#endif
