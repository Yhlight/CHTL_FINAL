#include "CHTLLoader.h"
#include "CHTLLexer.h"
#include "CHTLParser.h"
#include <fstream>

namespace CHTL {

CHTLLoader::CHTLLoader() {
}

NodePtr CHTLLoader::loadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        reportError("Cannot open file: " + filename);
        return nullptr;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    return loadString(content);
}

NodePtr CHTLLoader::loadString(const std::string& content) {
    try {
        // 词法分析
        CHTLLexer lexer(content);
        TokenList tokens = lexer.tokenize();
        
        // 语法分析
        CHTLParser parser(tokens);
        NodePtr root = parser.parse();
        
        return root;
    } catch (const std::exception& e) {
        reportError("Error loading content: " + std::string(e.what()));
        return nullptr;
    }
}

void CHTLLoader::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler_ = handler;
}

void CHTLLoader::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message);
    }
}

} // namespace CHTL