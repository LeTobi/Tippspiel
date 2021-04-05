#include "config.h"

#include <fstream>

std::string get_config(const std::string& file)
{
    std::fstream fs (file, fs.in);
    std::string line;
    if (!std::getline(fs,line))
        return "";
    return line;
}