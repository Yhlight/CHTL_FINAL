#include "FileLoaderNode.hpp"
#include <sstream>

namespace CHTL {

FileLoaderNode::FileLoaderNode(size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::FILELOADER, line, column) {}

void FileLoaderNode::addFile(const std::string& filePath) {
    files_.push_back(filePath);
}

std::string FileLoaderNode::toJavaScript() const {
    std::ostringstream oss;
    oss << "// FileLoader: AMD style JavaScript file loader\n";
    oss << "(function() {\n";
    oss << "    const files = [\n";
    
    for (size_t i = 0; i < files_.size(); ++i) {
        oss << "        \"" << files_[i] << "\"";
        if (i < files_.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "    ];\n";
    oss << "    \n";
    oss << "    // Load files asynchronously\n";
    oss << "    files.forEach(file => {\n";
    oss << "        const script = document.createElement('script');\n";
    oss << "        script.src = file;\n";
    oss << "        script.async = true;\n";
    oss << "        document.head.appendChild(script);\n";
    oss << "    });\n";
    oss << "})();\n";
    
    return oss.str();
}

std::string FileLoaderNode::toString() const {
    std::ostringstream oss;
    oss << "FileLoaderNode(files=[" << files_.size() << "], line=" << getLine() 
        << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL