#include "CHTLJSLexer.hpp"
#include <cctype>
#include <sstream>

namespace CHTL {

CHTLJSLexer::CHTLJSLexer(std::istream& input) 
    : input_(input), line_(1), column_(1) {
    current_ = nextToken();
}

CHTLJSToken CHTLJSLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(CHTLJSTokenType::END_OF_FILE, "");
    }
    
    char c = readChar();
    
    // 标识符和关键字
    if (std::isalpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 数字
    if (std::isdigit(c)) {
        return readNumberLiteral();
    }
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        return readStringLiteral();
    }
    
    // 增强选择器
    if (c == '{' && peekChar() == '{') {
        readChar(); // 消耗第二个 {
        return readEnhancedSelector();
    }
    
    // 操作符
    CHTLJSTokenType opType = checkOperator(c);
    if (opType != CHTLJSTokenType::ERROR) {
        return makeToken(opType, std::string(1, c));
    }
    
    // 未知字符
    return makeError("Unexpected character: " + std::string(1, c));
}

CHTLJSToken CHTLJSLexer::peekToken() {
    // 保存当前状态
    auto oldPos = input_.tellg();
    auto oldLine = line_;
    auto oldColumn = column_;
    auto oldCurrent = current_;
    
    // 读取下一个词法单元
    CHTLJSToken next = nextToken();
    
    // 恢复状态
    input_.seekg(oldPos);
    line_ = oldLine;
    column_ = oldColumn;
    current_ = oldCurrent;
    
    return next;
}

bool CHTLJSLexer::isAtEnd() const {
    return input_.eof();
}

char CHTLJSLexer::readChar() {
    char c = input_.get();
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

char CHTLJSLexer::peekChar() {
    char c = input_.peek();
    return c;
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peekChar();
        if (std::isspace(c)) {
            readChar();
        } else if (c == '/' && peekChar() == '/') {
            skipComment();
        } else {
            break;
        }
    }
}

void CHTLJSLexer::skipComment() {
    while (!isAtEnd() && readChar() != '\n') {
        // 跳过单行注释
    }
}

CHTLJSToken CHTLJSLexer::readIdentifier() {
    std::string value;
    input_.unget(); // 回退第一个字符
    value += static_cast<char>(input_.get()); // 重新读取第一个字符
    
    while (!isAtEnd()) {
        char c = peekChar();
        if (std::isalnum(c) || c == '_') {
            value += readChar();
        } else {
            break;
        }
    }
    
    CHTLJSTokenType type = checkKeyword(value);
    return makeToken(type, value);
}

CHTLJSToken CHTLJSLexer::readStringLiteral() {
    std::string value;
    char quote = readChar(); // 消耗开始引号
    
    while (!isAtEnd()) {
        char c = readChar();
        if (c == quote) {
            break;
        }
        value += c;
    }
    
    return makeToken(CHTLJSTokenType::STRING_LITERAL, value);
}

CHTLJSToken CHTLJSLexer::readUnquotedLiteral() {
    std::string value;
    
    while (!isAtEnd()) {
        char c = peekChar();
        if (std::isalnum(c) || c == '_' || c == '-' || c == '.') {
            value += readChar();
        } else {
            break;
        }
    }
    
    return makeToken(CHTLJSTokenType::UNQUOTED_LITERAL, value);
}

CHTLJSToken CHTLJSLexer::readNumberLiteral() {
    std::string value;
    input_.unget(); // 回退第一个字符
    value += static_cast<char>(input_.get()); // 重新读取第一个字符
    
    while (!isAtEnd()) {
        char c = peekChar();
        if (std::isdigit(c) || c == '.') {
            value += readChar();
        } else {
            break;
        }
    }
    
    return makeToken(CHTLJSTokenType::NUMBER_LITERAL, value);
}

CHTLJSToken CHTLJSLexer::readEnhancedSelector() {
    std::string value;
    
    while (!isAtEnd()) {
        char c = peekChar();
        if (c == '}' && peekChar() == '}') {
            readChar(); // 消耗第一个 }
            readChar(); // 消耗第二个 }
            break;
        }
        value += readChar();
    }
    
    return makeToken(CHTLJSTokenType::ENHANCED_SELECTOR_START, value);
}

CHTLJSTokenType CHTLJSLexer::checkKeyword(const std::string& word) {
    if (word == "fileloader") return CHTLJSTokenType::FILELOADER;
    if (word == "script") return CHTLJSTokenType::SCRIPT;
    if (word == "animate") return CHTLJSTokenType::ANIMATE;
    if (word == "listen") return CHTLJSTokenType::LISTEN;
    if (word == "util") return CHTLJSTokenType::UTIL;
    if (word == "change") return CHTLJSTokenType::CHANGE;
    if (word == "then") return CHTLJSTokenType::THEN;
    if (word == "if") return CHTLJSTokenType::IF;
    if (word == "else") return CHTLJSTokenType::ELSE;
    if (word == "for") return CHTLJSTokenType::FOR;
    if (word == "while") return CHTLJSTokenType::WHILE;
    if (word == "function") return CHTLJSTokenType::FUNCTION;
    if (word == "return") return CHTLJSTokenType::RETURN;
    if (word == "var") return CHTLJSTokenType::VAR;
    if (word == "let") return CHTLJSTokenType::LET;
    if (word == "const") return CHTLJSTokenType::CONST;
    
    return CHTLJSTokenType::IDENTIFIER;
}

CHTLJSTokenType CHTLJSLexer::checkOperator(char c) {
    switch (c) {
        case '=': return CHTLJSTokenType::ASSIGN;
        case ':': return CHTLJSTokenType::COLON;
        case ',': return CHTLJSTokenType::COMMA;
        case ';': return CHTLJSTokenType::SEMICOLON;
        case '.': return CHTLJSTokenType::DOT;
        case '|': return CHTLJSTokenType::PIPE;
        case '(': return CHTLJSTokenType::LEFT_PAREN;
        case ')': return CHTLJSTokenType::RIGHT_PAREN;
        case '{': return CHTLJSTokenType::LEFT_BRACE;
        case '}': return CHTLJSTokenType::RIGHT_BRACE;
        case '[': return CHTLJSTokenType::LEFT_BRACKET;
        case ']': return CHTLJSTokenType::RIGHT_BRACKET;
        case '&': return CHTLJSTokenType::REFERENCE;
        case '-': 
            if (peekChar() == '>') {
                readChar();
                return CHTLJSTokenType::ARROW;
            }
            break;
    }
    
    return CHTLJSTokenType::ERROR;
}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type, const std::string& value) {
    return CHTLJSToken(type, value, line_, column_);
}

CHTLJSToken CHTLJSLexer::makeError(const std::string& message) {
    return CHTLJSToken(CHTLJSTokenType::ERROR, message, line_, column_);
}

} // namespace CHTL