#include "CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLToken CHTLLexer::scanSymbol() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = current();
    advance();
    
    switch (c) {
        case '{': return CHTLToken(CHTLTokenType::LEFT_BRACE, "{", startLine, startColumn, start);
        case '}': return CHTLToken(CHTLTokenType::RIGHT_BRACE, "}", startLine, startColumn, start);
        case '(': return CHTLToken(CHTLTokenType::LEFT_PAREN, "(", startLine, startColumn, start);
        case ')': return CHTLToken(CHTLTokenType::RIGHT_PAREN, ")", startLine, startColumn, start);
        case '[': return CHTLToken(CHTLTokenType::LEFT_BRACKET, "[", startLine, startColumn, start);
        case ']': return CHTLToken(CHTLTokenType::RIGHT_BRACKET, "]", startLine, startColumn, start);
        case ';': return CHTLToken(CHTLTokenType::SEMICOLON, ";", startLine, startColumn, start);
        case ':': return CHTLToken(CHTLTokenType::COLON, ":", startLine, startColumn, start);
        case '=': return CHTLToken(CHTLTokenType::EQUAL, "=", startLine, startColumn, start);
        case ',': return CHTLToken(CHTLTokenType::COMMA, ",", startLine, startColumn, start);
        case '.': return CHTLToken(CHTLTokenType::DOT, ".", startLine, startColumn, start);
        case '&': return CHTLToken(CHTLTokenType::AMPERSAND, "&", startLine, startColumn, start);
        case '?': return CHTLToken(CHTLTokenType::QUESTION, "?", startLine, startColumn, start);
        case '|': return CHTLToken(CHTLTokenType::PIPE, "|", startLine, startColumn, start);
        case '@': {
            // 检查是否是模板关键字
            if (peek() == 'S' && peek(1) == 't' && peek(2) == 'y' && peek(3) == 'l' && peek(4) == 'e') {
                // 检查是否是 @Style
                advance(); advance(); advance(); advance(); advance(); // 跳过 "Style"
                return CHTLToken(CHTLTokenType::TEMPLATE_STYLE, "@Style", startLine, startColumn, start);
            } else if (peek() == 'E' && peek(1) == 'l' && peek(2) == 'e' && peek(3) == 'm' && peek(4) == 'e' && peek(5) == 'n' && peek(6) == 't') {
                // 检查是否是 @Element
                advance(); advance(); advance(); advance(); advance(); advance(); advance(); // 跳过 "Element"
                return CHTLToken(CHTLTokenType::TEMPLATE_ELEMENT, "@Element", startLine, startColumn, start);
            } else if (peek() == 'V' && peek(1) == 'a' && peek(2) == 'r') {
                // 检查是否是 @Var
                advance(); advance(); advance(); // 跳过 "Var"
                return CHTLToken(CHTLTokenType::TEMPLATE_VAR, "@Var", startLine, startColumn, start);
            } else {
                return CHTLToken(CHTLTokenType::AT, "@", startLine, startColumn, start);
            }
        }
        case '#': return CHTLToken(CHTLTokenType::HASH, "#", startLine, startColumn, start);
        case '$': return CHTLToken(CHTLTokenType::DOLLAR, "$", startLine, startColumn, start);
        case '~': return CHTLToken(CHTLTokenType::TILDE, "~", startLine, startColumn, start);
        case '^': return CHTLToken(CHTLTokenType::CARET, "^", startLine, startColumn, start);
        default: return createErrorToken("未知符号: " + std::string(1, c));
    }
}

CHTLToken CHTLLexer::scanOperator() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = current();
    advance();
    
    switch (c) {
        case '+': return CHTLToken(CHTLTokenType::PLUS, "+", startLine, startColumn, start);
        case '-': 
            if (current() == '>') {
                advance();
                return CHTLToken(CHTLTokenType::ARROW, "->", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::MINUS, "-", startLine, startColumn, start);
        case '*': 
            if (current() == '*') {
                advance();
                return CHTLToken(CHTLTokenType::POWER, "**", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::MULTIPLY, "*", startLine, startColumn, start);
        case '/': return CHTLToken(CHTLTokenType::DIVIDE, "/", startLine, startColumn, start);
        case '%': return CHTLToken(CHTLTokenType::MODULO, "%", startLine, startColumn, start);
        case '&': 
            if (current() == '&') {
                advance();
                return CHTLToken(CHTLTokenType::AND, "&&", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::AMPERSAND, "&", startLine, startColumn, start);
        case '|': 
            if (current() == '|') {
                advance();
                return CHTLToken(CHTLTokenType::OR, "||", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::PIPE, "|", startLine, startColumn, start);
        case '!': 
            if (current() == '=') {
                advance();
                return CHTLToken(CHTLTokenType::NOT_EQUAL, "!=", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::NOT, "!", startLine, startColumn, start);
        case '<': 
            if (current() == '=') {
                advance();
                return CHTLToken(CHTLTokenType::LESS_EQUAL, "<=", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::LESS, "<", startLine, startColumn, start);
        case '>': 
            if (current() == '=') {
                advance();
                return CHTLToken(CHTLTokenType::GREATER_EQUAL, ">=", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::GREATER, ">", startLine, startColumn, start);
        case '=': 
            if (current() == '=') {
                advance();
                return CHTLToken(CHTLTokenType::EQUAL_EQUAL, "==", startLine, startColumn, start);
            }
            return CHTLToken(CHTLTokenType::EQUAL, "=", startLine, startColumn, start);
        default: return createErrorToken("未知操作符: " + std::string(1, c));
    }
}

CHTLToken CHTLLexer::scanTemplateKeyword() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    // 检查 [Template] 或 [Custom]
    if (current() == '[') {
        advance();
        std::string keyword;
        while (isAlpha(current())) {
            keyword += current();
            advance();
        }
        if (current() == ']') {
            advance();
            if (keyword == "Template") {
                return CHTLToken(CHTLTokenType::TEMPLATE, "[Template]", startLine, startColumn, start);
            } else if (keyword == "Custom") {
                return CHTLToken(CHTLTokenType::CUSTOM, "[Custom]", startLine, startColumn, start);
            } else if (keyword == "Configuration") {
                return CHTLToken(CHTLTokenType::CONFIGURATION, "[Configuration]", startLine, startColumn, start);
            }
        }
    }
    
    return createErrorToken("无效的模板关键字");
}

CHTLToken CHTLLexer::scanOriginKeyword() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    if (current() == '@') {
        advance();
        std::string keyword;
        while (isAlpha(current())) {
            keyword += current();
            advance();
        }
        
        if (keyword == "Html") {
            return CHTLToken(CHTLTokenType::ORIGIN, "@Html", startLine, startColumn, start);
        } else if (keyword == "Style") {
            return CHTLToken(CHTLTokenType::TEMPLATE_STYLE, "@Style", startLine, startColumn, start);
        } else if (keyword == "Element") {
            return CHTLToken(CHTLTokenType::TEMPLATE_ELEMENT, "@Element", startLine, startColumn, start);
        } else if (keyword == "Var") {
            return CHTLToken(CHTLTokenType::TEMPLATE_VAR, "@Var", startLine, startColumn, start);
        } else if (keyword == "JavaScript") {
            return CHTLToken(CHTLTokenType::ORIGIN, "@JavaScript", startLine, startColumn, start);
        }
    }
    
    return createErrorToken("无效的原始嵌入关键字");
}

CHTLToken CHTLLexer::scanCHTLJSKeyword() {
    // CHTL JS关键字在scanIdentifier中处理
    return createErrorToken("CHTL JS关键字应在scanIdentifier中处理");
}

} // namespace CHTL