#include "CHTLLexer.h"
#include <cctype>
#include <stdexcept>
#include <sstream>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source) 
    : source(source), position(0), line(1), column(1), 
      inString(false), inComment(false), inMultiLineComment(false), 
      stringDelimiter('\0') {
}

char CHTLLexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLLexer::peekChar(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

void CHTLLexer::advance() {
    if (position < source.length()) {
        if (currentChar() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLLexer::skipWhitespace() {
    while (position < source.length() && isWhitespace(currentChar()) && !isNewline(currentChar())) {
        advance();
    }
}

void CHTLLexer::skipNewline() {
    while (position < source.length() && isNewline(currentChar())) {
        advance();
    }
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n';
}

bool CHTLLexer::isStringDelimiter(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isCommentStart() const {
    return currentChar() == '/' && peekChar() == '/';
}

bool CHTLLexer::isMultiLineCommentStart() const {
    return currentChar() == '/' && peekChar() == '*';
}

bool CHTLLexer::isMultiLineCommentEnd() const {
    return currentChar() == '*' && peekChar() == '/';
}

bool CHTLLexer::isGeneratorComment() const {
    return currentChar() == '-' && peekChar() == '-';
}

Token CHTLLexer::scanIdentifier() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    while (position < source.length() && isAlphaNumeric(currentChar())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    TokenType type = identifyKeyword(value);
    
    return Token(type, value, startLine, startColumn, start);
}

Token CHTLLexer::scanString() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    char delimiter = currentChar();
    
    advance(); // 跳过开始引号
    
    std::string value;
    while (position < source.length() && currentChar() != delimiter) {
        if (currentChar() == '\\') {
            advance();
            if (position < source.length()) {
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
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (position >= source.length()) {
        throwError("Unterminated string literal");
    }
    
    advance(); // 跳过结束引号
    return Token(TokenType::STRING, value, startLine, startColumn, start);
}

Token CHTLLexer::scanNumber() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    while (position < source.length() && (isDigit(currentChar()) || currentChar() == '.')) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return Token(TokenType::NUMBER, value, startLine, startColumn, start);
}

Token CHTLLexer::scanComment() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    if (isCommentStart()) {
        // 单行注释
        advance(); // 跳过第一个 '/'
        advance(); // 跳过第二个 '/'
        
        std::string value;
        while (position < source.length() && !isNewline(currentChar())) {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::COMMENT, value, startLine, startColumn, start);
    } else if (isMultiLineCommentStart()) {
        // 多行注释
        advance(); // 跳过第一个 '/'
        advance(); // 跳过 '*'
        
        std::string value;
        while (position < source.length() && !isMultiLineCommentEnd()) {
            value += currentChar();
            advance();
        }
        
        if (position >= source.length()) {
            throwError("Unterminated multi-line comment");
        }
        
        advance(); // 跳过 '*'
        advance(); // 跳过 '/'
        
        return Token(TokenType::COMMENT, value, startLine, startColumn, start);
    } else if (isGeneratorComment()) {
        // 生成器注释
        advance(); // 跳过第一个 '-'
        advance(); // 跳过第二个 '-'
        
        std::string value;
        while (position < source.length() && !isNewline(currentChar())) {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::COMMENT, value, startLine, startColumn, start);
    }
    
    return Token(TokenType::ERROR, "", startLine, startColumn, start);
}

Token CHTLLexer::scanOperator() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char first = currentChar();
    advance();
    
    std::string value(1, first);
    
    // 检查双字符运算符
    if (position < source.length()) {
        char second = currentChar();
        std::string twoChar = value + second;
        
        if (OPERATORS.find(twoChar) != OPERATORS.end()) {
            advance();
            value = twoChar;
        }
    }
    
    TokenType type = identifyOperator(value);
    return Token(type, value, startLine, startColumn, start);
}

Token CHTLLexer::scanDelimiter() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = currentChar();
    advance();
    
    std::string value(1, c);
    TokenType type = identifyDelimiter(value);
    
    return Token(type, value, startLine, startColumn, start);
}

Token CHTLLexer::scanSpecial() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = currentChar();
    advance();
    
    std::string value(1, c);
    TokenType type = TokenType::IDENTIFIER; // 默认为标识符
    
    switch (c) {
        case '@': type = TokenType::AT; break;
        case '#': type = TokenType::HASH; break;
        case '$': type = TokenType::DOLLAR; break;
        case '?': type = TokenType::QUESTION; break;
        case '!': type = TokenType::EXCLAMATION; break;
        case '&': type = TokenType::AMPERSAND; break;
        case '|': type = TokenType::PIPE; break;
        case '~': type = TokenType::TILDE; break;
        case '^': type = TokenType::CARET; break;
        default: type = TokenType::IDENTIFIER; break;
    }
    
    return Token(type, value, startLine, startColumn, start);
}

TokenType CHTLLexer::identifyKeyword(const std::string& value) const {
    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

TokenType CHTLLexer::identifyOperator(const std::string& value) const {
    auto it = OPERATORS.find(value);
    if (it != OPERATORS.end()) {
        return it->second;
    }
    return TokenType::ERROR;
}

TokenType CHTLLexer::identifyDelimiter(const std::string& value) const {
    auto it = DELIMITERS.find(value);
    if (it != DELIMITERS.end()) {
        return it->second;
    }
    return TokenType::ERROR;
}

TokenStream CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) {
            break;
        }
        
        char c = currentChar();
        
        if (isNewline(c)) {
            advance();
            tokens.push_back(Token(TokenType::NEWLINE, "\n", line, column, position));
            continue;
        }
        
        if (isStringDelimiter(c)) {
            tokens.push_back(scanString());
            continue;
        }
        
        if (isDigit(c)) {
            tokens.push_back(scanNumber());
            continue;
        }
        
        if (isCommentStart() || isMultiLineCommentStart() || isGeneratorComment()) {
            tokens.push_back(scanComment());
            continue;
        }
        
        if (isAlpha(c)) {
            tokens.push_back(scanIdentifier());
            continue;
        }
        
        // 检查运算符
        if (OPERATORS.find(std::string(1, c)) != OPERATORS.end()) {
            tokens.push_back(scanOperator());
            continue;
        }
        
        // 检查分隔符
        if (DELIMITERS.find(std::string(1, c)) != DELIMITERS.end()) {
            tokens.push_back(scanDelimiter());
            continue;
        }
        
        // 特殊字符
        if (c == '@' || c == '#' || c == '$' || c == '?' || c == '!' || 
            c == '&' || c == '|' || c == '~' || c == '^') {
            tokens.push_back(scanSpecial());
            continue;
        }
        
        // 未知字符
        throwError("Unexpected character: " + std::string(1, c));
    }
    
    // 添加EOF标记
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line, column, position));
    
    return TokenStream(tokens);
}

std::vector<Token> CHTLLexer::getAllTokens() {
    TokenStream stream = tokenize();
    std::vector<Token> tokens;
    
    while (stream.hasMore()) {
        tokens.push_back(stream.consume());
    }
    
    return tokens;
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

size_t CHTLLexer::getCurrentLine() const {
    return line;
}

size_t CHTLLexer::getCurrentColumn() const {
    return column;
}

size_t CHTLLexer::getCurrentPosition() const {
    return position;
}

void CHTLLexer::throwError(const std::string& message) const {
    std::ostringstream oss;
    oss << "Lexer error at line " << line << ", column " << column << ": " << message;
    throw std::runtime_error(oss.str());
}

} // namespace CHTL