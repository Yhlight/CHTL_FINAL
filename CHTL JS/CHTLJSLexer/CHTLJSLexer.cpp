#include "CHTLJSLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& source)
    : source_(source), position_(0), line_(1), column_(1) {
}

CHTLJSLexer::~CHTLJSLexer() {
}

std::vector<Token> CHTLJSLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (hasMoreTokens()) {
        Token token = nextToken();
        if (token.getType() != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

Token CHTLJSLexer::nextToken() {
    skipWhitespace();
    
    if (!hasMoreTokens()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_);
    }
    
    char c = currentChar();
    
    // 字符串
    if (c == '"' || c == '\'') {
        return parseString();
    }
    
    // 数字
    if (isDigit(c)) {
        return parseNumber();
    }
    
    // 增强选择器 {{
    if (c == '{' && peekChar() == '{') {
        return parseSelector();
    }
    
    // 响应式值 $JS变量名$
    if (c == '$') {
        return parseResponsiveValue();
    }
    
    // 标识符和关键字
    if (isLetter(c) || c == '_') {
        return parseIdentifier();
    }
    
    // 操作符和符号
    switch (c) {
        case '>':
            advance();
            if (currentChar() == '-') {
                advance();
                return Token(TokenType::ARROW, "->", line_, column_ - 1);
            }
            return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_);
            
        case '&':
            advance();
            if (currentChar() == '-' && peekChar() == '>') {
                advance();
                advance();
                return Token(TokenType::BIND_OP, "&->", line_, column_ - 2);
            }
            return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_);
            
        case '.':
            advance();
            return Token(TokenType::DOT, ".", line_, column_);
            
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", line_, column_);
            
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", line_, column_);
            
        case ':':
            advance();
            return Token(TokenType::COLON, ":", line_, column_);
            
        case '=':
            advance();
            return Token(TokenType::EQUAL, "=", line_, column_);
            
        case '?':
            advance();
            return Token(TokenType::QUESTION, "?", line_, column_);
            
        case '!':
            advance();
            return Token(TokenType::EXCLAMATION, "!", line_, column_);
            
        case '(':
            advance();
            return Token(TokenType::LEFT_PAREN, "(", line_, column_);
            
        case ')':
            advance();
            return Token(TokenType::RIGHT_PAREN, ")", line_, column_);
            
        case '{':
            advance();
            return Token(TokenType::LEFT_BRACE, "{", line_, column_);
            
        case '}':
            advance();
            return Token(TokenType::RIGHT_BRACE, "}", line_, column_);
            
        case '[':
            advance();
            return Token(TokenType::LEFT_BRACKET, "[", line_, column_);
            
        case ']':
            advance();
            return Token(TokenType::RIGHT_BRACKET, "]", line_, column_);
            
        default:
            advance();
            return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_);
    }
}

bool CHTLJSLexer::hasMoreTokens() const {
    return position_ < source_.length();
}

int CHTLJSLexer::getCurrentLine() const {
    return line_;
}

int CHTLJSLexer::getCurrentColumn() const {
    return column_;
}

const std::string& CHTLJSLexer::getSource() const {
    return source_;
}

char CHTLJSLexer::currentChar() const {
    if (position_ >= source_.length()) {
        return '\0';
    }
    return source_[position_];
}

char CHTLJSLexer::peekChar() const {
    if (position_ + 1 >= source_.length()) {
        return '\0';
    }
    return source_[position_ + 1];
}

void CHTLJSLexer::advance() {
    if (position_ < source_.length()) {
        if (source_[position_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLJSLexer::skipWhitespace() {
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLJSLexer::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        while (hasMoreTokens() && currentChar() != '\n') {
            advance();
        }
    } else if (currentChar() == '/' && peekChar() == '*') {
        advance(); // skip /
        advance(); // skip *
        while (hasMoreTokens()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // skip *
                advance(); // skip /
                break;
            }
            advance();
        }
    }
}

Token CHTLJSLexer::parseString() {
    char quote = currentChar();
    advance(); // skip opening quote
    
    std::string value;
    while (hasMoreTokens() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (hasMoreTokens()) {
                switch (currentChar()) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += currentChar(); break;
                }
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (hasMoreTokens()) {
        advance(); // skip closing quote
    }
    
    return Token(TokenType::STRING, value, line_, column_);
}

Token CHTLJSLexer::parseNumber() {
    std::string value;
    
    while (hasMoreTokens() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, line_, column_);
}

Token CHTLJSLexer::parseIdentifier() {
    std::string value;
    
    while (hasMoreTokens() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    
    TokenType type = getKeywordType(value);
    if (type != TokenType::UNKNOWN) {
        return Token(type, value, line_, column_);
    }
    
    return Token(TokenType::IDENTIFIER, value, line_, column_);
}

Token CHTLJSLexer::parseSelector() {
    advance(); // skip first {
    advance(); // skip second {
    
    std::string value;
    while (hasMoreTokens() && !(currentChar() == '}' && peekChar() == '}')) {
        value += currentChar();
        advance();
    }
    
    if (hasMoreTokens()) {
        advance(); // skip first }
        advance(); // skip second }
    }
    
    return Token(TokenType::SELECTOR_START, value, line_, column_);
}

Token CHTLJSLexer::parseResponsiveValue() {
    advance(); // skip $
    
    std::string value;
    while (hasMoreTokens() && currentChar() != '$') {
        value += currentChar();
        advance();
    }
    
    if (hasMoreTokens()) {
        advance(); // skip closing $
    }
    
    return Token(TokenType::RESPONSIVE_START, value, line_, column_);
}

bool CHTLJSLexer::isLetter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isLetter(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

TokenType CHTLJSLexer::getKeywordType(const std::string& keyword) const {
    static const std::map<std::string, TokenType> keywords = {
        {"ScriptLoader", TokenType::SCRIPT_LOADER},
        {"Listen", TokenType::LISTEN},
        {"Animate", TokenType::ANIMATE},
        {"Router", TokenType::ROUTER},
        {"Vir", TokenType::VIR},
        {"iNeverAway", TokenType::INEVERAWAY},
        {"util", TokenType::UTIL},
        {"printMylove", TokenType::PRINTMYLOVE}
    };
    
    auto it = keywords.find(keyword);
    if (it != keywords.end()) {
        return it->second;
    }
    
    return TokenType::UNKNOWN;
}

} // namespace CHTLJS