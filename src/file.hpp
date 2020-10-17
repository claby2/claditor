#ifndef CLADITOR_FILE_HPP
#define CLADITOR_FILE_HPP

#include <fstream>
#include <string>
#include <vector>

class File {
   public:
    explicit File(std::string);
    std::vector<std::string> get_content();
    void write_content(const std::vector<std::string>&);
    std::string get_path() const;

   private:
    std::string file_path_;
};

#endif
