#include "FileLoaderNode.hpp"
#include <sstream>

namespace CHTL {

FileLoaderNode::FileLoaderNode(size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::FILELOADER, line, column) {}

void FileLoaderNode::addLoadFile(const std::string& filePath) {
    loadFiles_.push_back(filePath);
}

std::string FileLoaderNode::toJavaScript() const {
    std::ostringstream oss;
    
    oss << "// AMD Style File Loader\n";
    oss << "define([";
    
    for (size_t i = 0; i < loadFiles_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << loadFiles_[i] << "\"";
    }
    
    oss << "], function() {\n";
    oss << "    // Loaded files: ";
    
    for (size_t i = 0; i < loadFiles_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << loadFiles_[i];
    }
    
    oss << "\n";
    oss << "    return {};\n";
    oss << "});\n";
    
    return oss.str();
}

std::string FileLoaderNode::toString() const {
    std::ostringstream oss;
    oss << "FileLoaderNode(files=" << loadFiles_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string FileLoaderNode::generateAMDLoader() const {
    return toJavaScript();
}

} // namespace CHTL