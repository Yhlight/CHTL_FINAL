#include "CHTLJSLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer() : position(0), line(1), column(1) {
}

CHTLJSLexer::~CHTLJSLexer() {
}

std::vector<Token> CHTLJSLexer::tokenize(const std::string& source) {
    setSource(source);
    std::vector<Token> tokens;
    
    while (hasMoreTokens()) {
        Token token = nextToken();
        if (token.getType() != TokenType::WHITESPACE && 
            token.getType() != TokenType::COMMENT) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line, column));
    
    return tokens;
}

void CHTLJSLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

void CHTLJSLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    errors.clear();
}

bool CHTLJSLexer::hasMoreTokens() const {
    return position < source.length();
}

Token CHTLJSLexer::nextToken() {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line, column);
    }
    
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line, column);
    }
    
    char c = currentChar();
    
    // 字符串
    if (c == '"' || c == '\'') {
        return readString();
    }
    
    // 数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 操作符
    if (isOperatorChar(c)) {
        return readOperator();
    }
    
    // 标点符号
    if (isPunctuationChar(c)) {
        return readPunctuation();
    }
    
    // 特殊符号
    if (c == '$' || c == '#' || c == '%' || c == '|' || c == '&') {
        return readSpecial();
    }
    
    // 注释
    if (c == '/' && peekChar() == '/') {
        skipComment();
        return nextToken();
    }
    
    // 未知字符
    reportError("Unknown character: " + std::string(1, c));
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
}

Token CHTLJSLexer::peekToken() {
    size_t oldPosition = position;
    size_t oldLine = line;
    size_t oldColumn = column;
    
    Token token = nextToken();
    
    position = oldPosition;
    line = oldLine;
    column = oldColumn;
    
    return token;
}

std::vector<std::string> CHTLJSLexer::getErrors() const {
    return errors;
}

bool CHTLJSLexer::hasErrors() const {
    return !errors.empty();
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        advance();
    }
}

void CHTLJSLexer::skipComment() {
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

Token CHTLJSLexer::readString() {
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                char escaped = currentChar();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
            }
        } else {
            value += currentChar();
        }
        advance();
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string literal");
        return Token(TokenType::STRING, value, line, column);
    }
    
    advance(); // 跳过结束引号
    return Token(TokenType::STRING, value, line, column);
}

Token CHTLJSLexer::readNumber() {
    std::string value;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, line, column);
}

Token CHTLJSLexer::readIdentifier() {
    std::string value;
    
    while (!isAtEnd() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    
    TokenType type = getKeywordType(value);
    return Token(type, value, line, column);
}

Token CHTLJSLexer::readOperator() {
    char c = currentChar();
    advance();
    
    if (c == '-' && currentChar() == '>') {
        advance();
        return Token(TokenType::ARROW, "->", line, column - 1);
    }
    
    if (c == '&' && currentChar() == '-' && peekChar() == '>') {
        advance();
        advance();
        return Token(TokenType::EVENT_ARROW, "&->", line, column - 2);
    }
    
    switch (c) {
        case '.': return Token(TokenType::DOT, ".", line, column);
        case ',': return Token(TokenType::COMMA, ",", line, column);
        case ':': return Token(TokenType::COLON, ":", line, column);
        case ';': return Token(TokenType::SEMICOLON, ";", line, column);
        case '=': return Token(TokenType::EQUAL, "=", line, column);
        case '?': return Token(TokenType::QUESTION, "?", line, column);
        case '!': return Token(TokenType::EXCLAMATION, "!", line, column);
        case '_': return Token(TokenType::UNDERSCORE, "_", line, column);
        default: return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
    }
}

Token CHTLJSLexer::readPunctuation() {
    char c = currentChar();
    advance();
    
    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", line, column);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", line, column);
        case '{': return Token(TokenType::LEFT_BRACE, "{", line, column);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", line, column);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", line, column);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", line, column);
        case '<': return Token(TokenType::LEFT_ANGLE, "<", line, column);
        case '>': return Token(TokenType::RIGHT_ANGLE, ">", line, column);
        default: return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
    }
}

Token CHTLJSLexer::readSpecial() {
    char c = currentChar();
    advance();
    
    switch (c) {
        case '$': return Token(TokenType::DOLLAR, "$", line, column);
        case '#': return Token(TokenType::HASH, "#", line, column);
        case '%': return Token(TokenType::PERCENT, "%", line, column);
        case '|': return Token(TokenType::PIPE, "|", line, column);
        case '&': return Token(TokenType::AMPERSAND, "&", line, column);
        default: return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
    }
}

char CHTLJSLexer::currentChar() const {
    return isAtEnd() ? '\0' : source[position];
}

char CHTLJSLexer::peekChar() const {
    return (position + 1 >= source.length()) ? '\0' : source[position + 1];
}

void CHTLJSLexer::advance() {
    if (!isAtEnd()) {
        position++;
        column++;
    }
}

bool CHTLJSLexer::isAtEnd() const {
    return position >= source.length();
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c) || c == '_';
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLJSLexer::isOperatorChar(char c) const {
    return c == '-' || c == '.' || c == ',' || c == ':' || 
           c == ';' || c == '=' || c == '?' || c == '!' || c == '_' || c == '&';
}

bool CHTLJSLexer::isPunctuationChar(char c) const {
    return c == '(' || c == ')' || c == '{' || c == '}' || 
           c == '[' || c == ']' || c == '<' || c == '>';
}

TokenType CHTLJSLexer::getKeywordType(const std::string& word) const {
    if (word == "Vir") return TokenType::VIR;
    if (word == "Listen") return TokenType::LISTEN;
    if (word == "Animate") return TokenType::ANIMATE;
    if (word == "Router") return TokenType::ROUTER;
    if (word == "ScriptLoader") return TokenType::SCRIPT_LOADER;
    if (word == "Delegate") return TokenType::DELEGATE;
    if (word == "iNeverAway") return TokenType::INEVERAWAY;
    if (word == "util") return TokenType::UTIL;
    if (word == "change") return TokenType::CHANGE;
    if (word == "then") return TokenType::THEN;
    if (word == "true" || word == "false") return TokenType::BOOLEAN;
    
    return TokenType::IDENTIFIER;
}

bool CHTLJSLexer::isKeyword(const std::string& word) const {
    return getKeywordType(word) != TokenType::IDENTIFIER;
}

void CHTLJSLexer::reportError(const std::string& message) {
    std::string error = "Error at line " + std::to_string(line) + 
                       ", column " + std::to_string(column) + ": " + message;
    errors.push_back(error);
}

} // namespace CHTLJS