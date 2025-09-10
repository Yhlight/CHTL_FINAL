#include "FileSystemUtil.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace FileSystemUtil {

    std::string readFile(const std::string& filePath) {
        std::ifstream fileStream(filePath);
        if (!fileStream) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    bool writeFile(const std::string& filePath, const std::string& content) {
        std::ofstream fileStream(filePath);
        if (!fileStream) {
            return false; // Could not open file for writing
        }
        fileStream << content;
        return fileStream.good();
    }

} // namespace FileSystemUtil
