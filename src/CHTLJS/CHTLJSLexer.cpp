#include "CHTLJS/CHTLJSLexer.h"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLJSLexer::CHTLJSLexer() {
}

CHTLJSLexer::~CHTLJSLexer() = default;

std::vector<std::string> CHTLJSLexer::tokenize(const std::string& code) {
    std::vector<std::string> tokens;
    std::string currentToken;
    bool inString = false;
    bool inComment = false;
    bool inBlockComment = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        
        if (inComment) {
            if (c == '\n') {
                inComment = false;
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
            } else {
                currentToken += c;
            }
            continue;
        }
        
        if (inBlockComment) {
            if (c == '*' && i + 1 < code.length() && code[i + 1] == '/') {
                inBlockComment = false;
                i++; // 跳过 '/'
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
            } else {
                currentToken += c;
            }
            continue;
        }
        
        if (inString) {
            if (c == stringChar) {
                inString = false;
                currentToken += c;
                tokens.push_back(currentToken);
                currentToken.clear();
            } else {
                currentToken += c;
            }
            continue;
        }
        
        if (c == '"' || c == '\'') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            inString = true;
            stringChar = c;
            currentToken += c;
        } else if (c == '/' && i + 1 < code.length() && code[i + 1] == '/') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            inComment = true;
            currentToken += c;
        } else if (c == '/' && i + 1 < code.length() && code[i + 1] == '*') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            inBlockComment = true;
            currentToken += c;
        } else if (std::isspace(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else if (isSpecialCharacter(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    return tokens;
}

bool CHTLJSLexer::isSpecialCharacter(char c) const {
    static const std::string specialChars = "{}[]();,.:+-*/%=<>!&|^~?";
    return specialChars.find(c) != std::string::npos;
}

} // namespace CHTL