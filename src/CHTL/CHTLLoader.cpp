#include "CHTL/CHTLLoader.h"
#include <fstream>
#include <sstream>

namespace CHTL {

CHTLLoader::CHTLLoader(std::shared_ptr<CHTLContext> context) 
    : m_context(context) {
}

CHTLLoader::~CHTLLoader() = default;

std::string CHTLLoader::loadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Failed to open file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

bool CHTLLoader::saveFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Failed to create file: " + filePath);
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

std::vector<std::string> CHTLLoader::loadDirectory(const std::string& dirPath) {
    // TODO: 实现目录加载
    return {};
}

bool CHTLLoader::fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

std::string CHTLLoader::getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filePath.substr(dotPos + 1);
    }
    return "";
}

} // namespace CHTL