#include "Loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Loader::readTextFile(const std::string& path) {
    // For now, assume path is relative to the execution directory.
    // The spec requires a more complex search path logic.
    std::ifstream file(path);
    if (!file.is_open()) {
        // In a real compiler, we might want to handle this more gracefully.
        std::cerr << "Warning: Could not open imported file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
