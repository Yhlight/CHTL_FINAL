#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

class Loader {
public:
    static std::string loadFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Loader Error: Could not open file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

} // namespace CHTL
