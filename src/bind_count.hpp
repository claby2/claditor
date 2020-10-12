#ifndef CLADITOR_BIND_COUNT_HPP
#define CLADITOR_BIND_COUNT_HPP

class BindCount {
   public:
    BindCount();
    int get_value();
    bool empty() const;
    void add_digit(int);
    void reset();

   private:
    int value_;
};
#endif
