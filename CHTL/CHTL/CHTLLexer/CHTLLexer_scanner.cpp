#include "CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLToken CHTLLexer::scanIdentifier() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    while (isAlphaNumeric(current())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    CHTLTokenType type = CHTLTokenType::IDENTIFIER;
    
    // 检查是否是关键字
    if (keywords.find(value) != keywords.end()) {
        type = keywords[value];
    }
    
    return CHTLToken(type, value, startLine, startColumn, start);
}

CHTLToken CHTLLexer::scanString() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    char quote = current();
    advance(); // 跳过开始引号
    
    std::string value;
    while (current() != quote && current() != '\0') {
        if (current() == '\\') {
            advance();
            switch (current()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += current(); break;
            }
        } else {
            value += current();
        }
        advance();
    }
    
    if (current() == quote) {
        advance(); // 跳过结束引号
    } else {
        addError("未闭合的字符串");
    }
    
    return CHTLToken(CHTLTokenType::STRING, value, startLine, startColumn, start);
}

CHTLToken CHTLLexer::scanNumber() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    // 处理整数部分
    while (isDigit(current())) {
        value += current();
        advance();
    }
    
    // 处理小数部分
    if (current() == '.' && isDigit(peek())) {
        value += current();
        advance();
        while (isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    // 处理科学计数法
    if (current() == 'e' || current() == 'E') {
        value += current();
        advance();
        if (current() == '+' || current() == '-') {
            value += current();
            advance();
        }
        while (isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    return CHTLToken(CHTLTokenType::NUMBER, value, startLine, startColumn, start);
}

CHTLToken CHTLLexer::scanUnquotedLiteral() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (position < source.length() && 
           !isWhitespace(current()) && 
           current() != ';' && 
           current() != ':' && 
           current() != '=' && 
           current() != '{' && 
           current() != '}' &&
           current() != ',' &&
           current() != '\n' &&
           current() != '+' &&
           current() != '*' &&
           current() != '/' &&
           current() != '%' &&
           current() != '&' &&
           current() != '|' &&
           current() != '!' &&
           current() != '<' &&
           current() != '>' &&
           current() != '?' &&
           current() != '^' &&
           current() != '~') {
        value += current();
        advance();
    }
    
    return CHTLToken(CHTLTokenType::UNQUOTED_LITERAL, value, startLine, startColumn, start);
}

CHTLToken CHTLLexer::scanComment() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    if (current() == '/' && peek() == '/') {
        // 单行注释
        advance(); // 跳过第一个 /
        advance(); // 跳过第二个 /
        
        std::string value;
        while (current() != '\n' && current() != '\0') {
            value += current();
            advance();
        }
        
        return CHTLToken(CHTLTokenType::LINE_COMMENT, value, startLine, startColumn, start);
    } else if (current() == '/' && peek() == '*') {
        // 多行注释
        advance(); // 跳过 /
        advance(); // 跳过 *
        
        std::string value;
        while (position < source.length()) {
            if (current() == '*' && peek() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                break;
            }
            value += current();
            advance();
        }
        
        return CHTLToken(CHTLTokenType::BLOCK_COMMENT, value, startLine, startColumn, start);
    } else if (current() == '-' && peek() == '-') {
        // 生成器注释
        advance(); // 跳过第一个 -
        advance(); // 跳过第二个 -
        
        std::string value;
        while (current() != '\n' && current() != '\0') {
            value += current();
            advance();
        }
        
        return CHTLToken(CHTLTokenType::GENERATOR_COMMENT, value, startLine, startColumn, start);
    }
    
    return createErrorToken("无效的注释");
}

} // namespace CHTL