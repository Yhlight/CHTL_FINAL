#include "Lexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& source) 
    : source_(source), current_pos_(0), current_line_(1), current_column_(1) {
}

std::unique_ptr<Token> Lexer::nextToken() {
    skipWhitespace();
    
    if (current_pos_ >= source_.length()) {
        return std::make_unique<Token>(TokenType::END_OF_FILE, "", current_line_, current_column_);
    }
    
    char c = currentChar();
    size_t line = current_line_;
    size_t column = current_column_;
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        nextChar(); // 跳过开始引号
        std::string value = readString(c);
        return std::make_unique<Token>(TokenType::STRING, value, line, column);
    }
    
    // 处理数字
    if (isDigit(c)) {
        std::string value = readNumber();
        return std::make_unique<Token>(TokenType::NUMBER, value, line, column);
    }
    
    // 处理标识符和关键字
    if (isAlpha(c) || c == '_') {
        std::string value = readIdentifier();
        TokenType type = isKeyword(value) ? KeywordMap::getKeywordType(value) : TokenType::IDENTIFIER;
        return std::make_unique<Token>(type, value, line, column);
    }
    
    // 处理特殊符号
    std::string symbol(1, c);
    
    // 检查双字符符号
    if (current_pos_ + 1 < source_.length()) {
        char next = source_[current_pos_ + 1];
        std::string doubleSymbol = symbol + next;
        
        TokenType doubleType = getSpecialSymbol(doubleSymbol);
        if (doubleType != TokenType::UNKNOWN) {
            nextChar(); // 消费第一个字符
            nextChar(); // 消费第二个字符
            return std::make_unique<Token>(doubleType, doubleSymbol, line, column);
        }
    }
    
    // 检查单字符符号
    TokenType singleType = getSpecialSymbol(symbol);
    if (singleType != TokenType::UNKNOWN) {
        nextChar();
        return std::make_unique<Token>(singleType, symbol, line, column);
    }
    
    // 处理字面量（无修饰字符串）
    if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '{' && c != '}' && 
        c != '[' && c != ']' && c != '(' && c != ')' && c != ';' && c != ':' && 
        c != '=' && c != ',' && c != '.' && c != '#' && c != '&' && c != '/' && c != '*') {
        std::string value = readLiteral();
        return std::make_unique<Token>(TokenType::LITERAL, value, line, column);
    }
    
    // 未知字符
    nextChar();
    return std::make_unique<Token>(TokenType::UNKNOWN, symbol, line, column);
}

std::unique_ptr<Token> Lexer::peekToken() {
    size_t saved_pos = current_pos_;
    size_t saved_line = current_line_;
    size_t saved_column = current_column_;
    
    auto token = nextToken();
    
    current_pos_ = saved_pos;
    current_line_ = saved_line;
    current_column_ = saved_column;
    
    return token;
}

bool Lexer::hasMoreTokens() const {
    return current_pos_ < source_.length();
}

void Lexer::reset(size_t position) {
    current_pos_ = position;
    current_line_ = 1;
    current_column_ = 1;
    
    // 重新计算行号和列号
    for (size_t i = 0; i < position && i < source_.length(); ++i) {
        if (source_[i] == '\n') {
            current_line_++;
            current_column_ = 1;
        } else {
            current_column_++;
        }
    }
}

void Lexer::skipWhitespace() {
    while (current_pos_ < source_.length() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            current_line_++;
            current_column_ = 1;
        } else {
            current_column_++;
        }
        current_pos_++;
    }
}

void Lexer::skipComment() {
    if (current_pos_ + 1 < source_.length()) {
        char c1 = currentChar();
        char c2 = source_[current_pos_ + 1];
        
        if (c1 == '/' && c2 == '/') {
            // 单行注释
            while (current_pos_ < source_.length() && currentChar() != '\n') {
                current_pos_++;
            }
            if (current_pos_ < source_.length()) {
                current_pos_++; // 跳过换行符
                current_line_++;
                current_column_ = 1;
            }
        } else if (c1 == '/' && c2 == '*') {
            // 块注释
            current_pos_ += 2; // 跳过 /*
            while (current_pos_ + 1 < source_.length()) {
                if (currentChar() == '*' && source_[current_pos_ + 1] == '/') {
                    current_pos_ += 2; // 跳过 */
                    break;
                }
                if (currentChar() == '\n') {
                    current_line_++;
                    current_column_ = 1;
                } else {
                    current_column_++;
                }
                current_pos_++;
            }
        } else if (c1 == '-' && c2 == '-') {
            // 生成器注释
            while (current_pos_ < source_.length() && currentChar() != '\n') {
                current_pos_++;
            }
            if (current_pos_ < source_.length()) {
                current_pos_++; // 跳过换行符
                current_line_++;
                current_column_ = 1;
            }
        }
    }
}

std::string Lexer::readString(char delimiter) {
    std::string result;
    while (current_pos_ < source_.length() && currentChar() != delimiter) {
        if (currentChar() == '\\' && current_pos_ + 1 < source_.length()) {
            // 处理转义字符
            current_pos_++;
            char escaped = currentChar();
            switch (escaped) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += escaped; break;
            }
        } else {
            result += currentChar();
        }
        current_pos_++;
    }
    if (current_pos_ < source_.length()) {
        current_pos_++; // 跳过结束引号
    }
    return result;
}

std::string Lexer::readNumber() {
    std::string result;
    while (current_pos_ < source_.length() && (isDigit(currentChar()) || currentChar() == '.')) {
        result += currentChar();
        current_pos_++;
    }
    return result;
}

std::string Lexer::readIdentifier() {
    std::string result;
    while (current_pos_ < source_.length() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        result += currentChar();
        current_pos_++;
    }
    return result;
}

std::string Lexer::readLiteral() {
    std::string result;
    while (current_pos_ < source_.length() && 
           !isWhitespace(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' &&
           currentChar() != '[' && currentChar() != ']' &&
           currentChar() != '(' && currentChar() != ')' &&
           currentChar() != ';' && currentChar() != ':' &&
           currentChar() != '=' && currentChar() != ',' &&
           currentChar() != '.' && currentChar() != '#' &&
           currentChar() != '&' && currentChar() != '/' &&
           currentChar() != '*') {
        result += currentChar();
        current_pos_++;
    }
    return result;
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char Lexer::currentChar() const {
    return (current_pos_ < source_.length()) ? source_[current_pos_] : '\0';
}

char Lexer::nextChar() {
    if (current_pos_ < source_.length()) {
        char c = source_[current_pos_];
        current_pos_++;
        if (c == '\n') {
            current_line_++;
            current_column_ = 1;
        } else {
            current_column_++;
        }
        return c;
    }
    return '\0';
}

char Lexer::peekChar() const {
    return (current_pos_ + 1 < source_.length()) ? source_[current_pos_ + 1] : '\0';
}

bool Lexer::isKeyword(const std::string& word) const {
    return KeywordMap::isKeyword(word);
}

TokenType Lexer::getSpecialSymbol(const std::string& symbol) const {
    static const std::unordered_map<std::string, TokenType> symbols = {
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {";", TokenType::SEMICOLON},
        {":", TokenType::COLON},
        {"=", TokenType::EQUALS},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"#", TokenType::HASH},
        {"&", TokenType::AMPERSAND},
        {"->", TokenType::ARROW},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"<", TokenType::LESS},
        {">", TokenType::GREATER},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"==", TokenType::EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"!", TokenType::NOT},
        {"?", TokenType::QUESTION}
    };
    
    auto it = symbols.find(symbol);
    return (it != symbols.end()) ? it->second : TokenType::UNKNOWN;
}

} // namespace CHTL