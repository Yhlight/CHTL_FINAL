#include "CHTLJSIOStream.h"
#include <filesystem>
#include <iostream>

namespace CHTL {

CHTLJSIOStream::CHTLJSIOStream() : fileStream(std::make_unique<std::fstream>()) {}

CHTLJSIOStream::~CHTLJSIOStream() {
    close();
}

bool CHTLJSIOStream::open(const std::string& filePath, std::ios_base::openmode mode) {
    close();
    
    fileStream->open(filePath, mode);
    if (fileStream->is_open()) {
        currentFilePath = filePath;
        return true;
    }
    
    return false;
}

void CHTLJSIOStream::close() {
    if (fileStream && fileStream->is_open()) {
        fileStream->close();
    }
    currentFilePath.clear();
}

bool CHTLJSIOStream::isOpen() const {
    return fileStream && fileStream->is_open();
}

std::string CHTLJSIOStream::readLine() {
    if (!isOpen()) {
        return "";
    }
    
    std::string line;
    if (std::getline(*fileStream, line)) {
        return line;
    }
    
    return "";
}

std::string CHTLJSIOStream::readAll() {
    if (!isOpen()) {
        return "";
    }
    
    std::string content;
    std::string line;
    
    // 重置文件指针到开始
    fileStream->seekg(0, std::ios::beg);
    
    while (std::getline(*fileStream, line)) {
        content += line + "\n";
    }
    
    return content;
}

bool CHTLJSIOStream::write(const std::string& content) {
    if (!isOpen()) {
        return false;
    }
    
    *fileStream << content;
    return fileStream->good();
}

bool CHTLJSIOStream::writeLine(const std::string& line) {
    if (!isOpen()) {
        return false;
    }
    
    *fileStream << line << std::endl;
    return fileStream->good();
}

size_t CHTLJSIOStream::getFileSize() const {
    if (!isOpen()) {
        return 0;
    }
    
    // 保存当前位置
    std::streampos currentPos = fileStream->tellg();
    
    // 移动到文件末尾
    fileStream->seekg(0, std::ios::end);
    std::streampos endPos = fileStream->tellg();
    
    // 恢复原位置
    fileStream->seekg(currentPos);
    
    return static_cast<size_t>(endPos);
}

bool CHTLJSIOStream::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

bool CHTLJSIOStream::createDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::create_directories(dirPath);
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

} // namespace CHTL