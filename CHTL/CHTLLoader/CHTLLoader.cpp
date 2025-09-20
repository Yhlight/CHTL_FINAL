#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

std::string CHTLLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("CHTLLoader Error: Could not open file at path: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace CHTL
