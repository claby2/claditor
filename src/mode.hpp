#ifndef CLADITOR_MODE_HPP
#define CLADITOR_MODE_HPP

enum class ModeType { EXIT, NORMAL, INSERT, VISUAL, COMMAND };

class Mode {
   public:
    explicit Mode(ModeType type);
    ModeType get_type() const;
    void set_type(ModeType type);

   private:
    ModeType type_;
};
#endif
