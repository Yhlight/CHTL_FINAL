#include "CHTLIOStream.h"
#include <filesystem>
#include <sstream>

namespace CHTL {

CHTLIOStream::CHTLIOStream() : debugMode(false) {
}

CHTLIOStream::CHTLIOStream(const std::string& input, const std::string& output) 
    : inputPath(input), outputPath(output), debugMode(false) {
}

std::string CHTLIOStream::readInput() {
    if (inputPath.empty()) {
        return "";
    }
    
    return readFile(inputPath);
}

std::string CHTLIOStream::readFile(const std::string& filename) {
    if (!fileExists(filename)) {
        if (debugMode) {
            std::cerr << "File not found: " << filename << std::endl;
        }
        return "";
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        if (debugMode) {
            std::cerr << "Cannot open file: " << filename << std::endl;
        }
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    
    return content.str();
}

bool CHTLIOStream::hasInput() const {
    return !inputPath.empty() && fileExists(inputPath);
}

void CHTLIOStream::writeOutput(const std::string& content) {
    if (outputPath.empty()) {
        if (debugMode) {
            std::cerr << "No output path specified" << std::endl;
        }
        return;
    }
    
    writeFile(outputPath, content);
}

void CHTLIOStream::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        if (debugMode) {
            std::cerr << "Cannot create file: " << filename << std::endl;
        }
        return;
    }
    
    file << content;
    file.close();
}

void CHTLIOStream::appendOutput(const std::string& content) {
    if (outputPath.empty()) {
        if (debugMode) {
            std::cerr << "No output path specified" << std::endl;
        }
        return;
    }
    
    std::ofstream file(outputPath, std::ios::app);
    if (!file.is_open()) {
        if (debugMode) {
            std::cerr << "Cannot append to file: " << outputPath << std::endl;
        }
        return;
    }
    
    file << content;
    file.close();
}

void CHTLIOStream::clearBuffer() {
    buffer.str("");
    buffer.clear();
}

std::string CHTLIOStream::getBufferContent() const {
    return buffer.str();
}

void CHTLIOStream::setBufferContent(const std::string& content) {
    buffer.str(content);
    buffer.clear();
}

bool CHTLIOStream::fileExists(const std::string& filename) const {
    return std::filesystem::exists(filename);
}

std::string CHTLIOStream::getFileExtension(const std::string& filename) const {
    std::filesystem::path file(filename);
    return file.extension().string();
}

std::string CHTLIOStream::getFileName(const std::string& path) const {
    std::filesystem::path file(path);
    return file.filename().string();
}

std::string CHTLIOStream::getDirectory(const std::string& path) const {
    std::filesystem::path file(path);
    return file.parent_path().string();
}

std::string CHTLIOStream::toString() const {
    std::ostringstream oss;
    oss << "CHTLIOStream{";
    oss << "inputPath=" << inputPath << ", ";
    oss << "outputPath=" << outputPath << ", ";
    oss << "bufferSize=" << buffer.str().length() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string CHTLIOStream::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLIOStream Debug Info:\n";
    oss << "  Input Path: " << inputPath << "\n";
    oss << "  Output Path: " << outputPath << "\n";
    oss << "  Buffer Size: " << buffer.str().length() << "\n";
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    return oss.str();
}

std::string CHTLIOStream::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"inputPath\": \"" << inputPath << "\",\n";
    oss << "  \"outputPath\": \"" << outputPath << "\",\n";
    oss << "  \"bufferSize\": " << buffer.str().length() << ",\n";
    oss << "  \"debugMode\": " << (debugMode ? "true" : "false") << "\n";
    oss << "}";
    return oss.str();
}

std::string CHTLIOStream::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CHTLIOStream>\n";
    oss << "  <inputPath>" << inputPath << "</inputPath>\n";
    oss << "  <outputPath>" << outputPath << "</outputPath>\n";
    oss << "  <bufferSize>" << buffer.str().length() << "</bufferSize>\n";
    oss << "  <debugMode>" << (debugMode ? "true" : "false") << "</debugMode>\n";
    oss << "</CHTLIOStream>";
    return oss.str();
}

std::string CHTLIOStream::toYAML() const {
    std::ostringstream oss;
    oss << "CHTLIOStream:\n";
    oss << "  inputPath: " << inputPath << "\n";
    oss << "  outputPath: " << outputPath << "\n";
    oss << "  bufferSize: " << buffer.str().length() << "\n";
    oss << "  debugMode: " << (debugMode ? "true" : "false") << "\n";
    return oss.str();
}

std::shared_ptr<CHTLIOStream> CHTLIOStream::clone() const {
    auto cloned = std::make_shared<CHTLIOStream>();
    cloned->inputPath = inputPath;
    cloned->outputPath = outputPath;
    cloned->buffer << buffer.str();
    cloned->debugMode = debugMode;
    return cloned;
}

bool CHTLIOStream::operator==(const CHTLIOStream& other) const {
    return inputPath == other.inputPath &&
           outputPath == other.outputPath &&
           buffer.str() == other.buffer.str() &&
           debugMode == other.debugMode;
}

bool CHTLIOStream::operator!=(const CHTLIOStream& other) const {
    return !(*this == other);
}

} // namespace CHTL