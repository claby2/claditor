#ifndef CLADITOR_FILE_HPP
#define CLADITOR_FILE_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class File {
   public:
    File(const std::string &, const std::stringstream &);
    std::vector<std::string> get_content();
    void write_content(const std::vector<std::string> &);
    std::string get_path() const;

   private:
    std::string file_path_;
    std::stringstream file_stream_;
};

#endif
