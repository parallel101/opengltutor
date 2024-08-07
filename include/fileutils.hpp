#pragma once

#include <fstream>
#include <iostream>
#include <string>

inline std::string get_file_content(std::string const &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << '\n';
        return {};
    }
    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()};
}
