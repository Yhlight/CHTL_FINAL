#include "CHTL/CHTLIOStream.h"
#include <fstream>
#include <sstream>

namespace CHTL {

CHTLIOStream::CHTLIOStream() {
}

CHTLIOStream::~CHTLIOStream() = default;

std::string CHTLIOStream::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

bool CHTLIOStream::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

std::vector<std::string> CHTLIOStream::readLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        return lines;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    file.close();
    return lines;
}

bool CHTLIOStream::writeLines(const std::string& filePath, const std::vector<std::string>& lines) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& line : lines) {
        file << line << std::endl;
    }
    
    file.close();
    return true;
}

} // namespace CHTL