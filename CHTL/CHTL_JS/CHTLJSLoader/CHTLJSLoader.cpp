#include "CHTLJSLoader.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace CHTL_JS {

CHTLJSLoader::CHTLJSLoader() {
}

CHTLJSLoader::CHTLJSLoader(const std::string& basePath) : basePath(basePath) {
}

std::string CHTLJSLoader::loadFile(const std::string& filename) {
    std::string fullPath = resolvePath(filename);
    
    if (!fileExists(fullPath)) {
        errors.push_back("File not found: " + fullPath);
        return "";
    }
    
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        errors.push_back("Cannot open file: " + fullPath);
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    file.close();
    
    // 添加到已加载文件列表
    if (std::find(loadedFiles.begin(), loadedFiles.end(), filename) == loadedFiles.end()) {
        loadedFiles.push_back(filename);
    }
    
    return content;
}

std::vector<std::string> CHTLJSLoader::loadFiles(const std::vector<std::string>& filenames) {
    std::vector<std::string> contents;
    
    for (const auto& filename : filenames) {
        std::string content = loadFile(filename);
        contents.push_back(content);
    }
    
    return contents;
}

bool CHTLJSLoader::isFileLoaded(const std::string& filename) const {
    return std::find(loadedFiles.begin(), loadedFiles.end(), filename) != loadedFiles.end();
}

void CHTLJSLoader::clearErrors() {
    errors.clear();
}

void CHTLJSLoader::clearLoadedFiles() {
    loadedFiles.clear();
}

std::string CHTLJSLoader::resolvePath(const std::string& filename) const {
    if (basePath.empty()) {
        return filename;
    }
    
    std::filesystem::path base(basePath);
    std::filesystem::path file(filename);
    
    if (file.is_absolute()) {
        return filename;
    }
    
    return (base / file).string();
}

bool CHTLJSLoader::fileExists(const std::string& filename) const {
    std::string fullPath = resolvePath(filename);
    return std::filesystem::exists(fullPath);
}

std::string CHTLJSLoader::getFileExtension(const std::string& filename) const {
    std::filesystem::path file(filename);
    return file.extension().string();
}

std::string CHTLJSLoader::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSLoader{";
    oss << "basePath=" << basePath << ", ";
    oss << "loadedFiles=" << loadedFiles.size() << ", ";
    oss << "errors=" << errors.size();
    oss << "}";
    return oss.str();
}

std::string CHTLJSLoader::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSLoader Debug Info:\n";
    oss << "  Base Path: " << basePath << "\n";
    oss << "  Loaded Files: " << loadedFiles.size() << "\n";
    for (const auto& file : loadedFiles) {
        oss << "    " << file << "\n";
    }
    oss << "  Errors: " << errors.size() << "\n";
    for (const auto& error : errors) {
        oss << "    " << error << "\n";
    }
    return oss.str();
}

} // namespace CHTL_JS