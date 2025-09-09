#include "CHTL/CHTLLexer/Lexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& source) 
    : source_(source), currentPosition_(0), currentLine_(1), 
      currentColumn_(1), tokenIndex_(0) {
}

Lexer::~Lexer() = default;

TokenList Lexer::tokenize() {
    tokens_.clear();
    currentPosition_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    
    while (!isEOF()) {
        skipWhitespace();
        if (isEOF()) break;
        
        Token token = nextToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens_.push_back(token);
        }
    }
    
    // 添加文件结束标记
    tokens_.emplace_back(TokenType::END_OF_FILE, "", currentLine_, currentColumn_, currentPosition_);
    tokenIndex_ = 0;
    
    return tokens_;
}

Token Lexer::nextToken() {
    if (tokenIndex_ < tokens_.size()) {
        return tokens_[tokenIndex_++];
    }
    
    if (isEOF()) {
        return Token(TokenType::END_OF_FILE, "", currentLine_, currentColumn_, currentPosition_);
    }
    
    skipWhitespace();
    if (isEOF()) {
        return Token(TokenType::END_OF_FILE, "", currentLine_, currentColumn_, currentPosition_);
    }
    
    char c = currentChar();
    
    // 检查注释
    if (c == '/' && nextChar() == '/') {
        ungetChar();
        return readGeneratorComment();
    }
    if (c == '/' && nextChar() == '*') {
        ungetChar();
        return readGeneratorComment();
    }
    if (c == '-' && nextChar() == '-') {
        ungetChar();
        return readGeneratorComment();
    }
    
    // 检查标识符和关键字
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 检查字符串
    if (c == '"' || c == '\'') {
        return readString();
    }
    
    // 检查数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 检查符号
    return readSymbol();
}

Token Lexer::peekToken() {
    size_t oldIndex = tokenIndex_;
    Token token = nextToken();
    tokenIndex_ = oldIndex;
    return token;
}

void Lexer::ungetToken() {
    if (tokenIndex_ > 0) {
        --tokenIndex_;
    }
}

bool Lexer::isEOF() const {
    return currentPosition_ >= source_.length();
}

void Lexer::skipWhitespace() {
    while (!isEOF() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            ++currentLine_;
            currentColumn_ = 1;
        } else {
            ++currentColumn_;
        }
        ++currentPosition_;
    }
}

void Lexer::skipLineComment() {
    while (!isEOF() && currentChar() != '\n') {
        ++currentPosition_;
        ++currentColumn_;
    }
    if (!isEOF()) {
        ++currentPosition_;
        ++currentLine_;
        currentColumn_ = 1;
    }
}

void Lexer::skipBlockComment() {
    while (!isEOF()) {
        if (currentChar() == '*' && nextChar() == '/') {
            ++currentPosition_;
            ++currentColumn_;
            break;
        }
        if (currentChar() == '\n') {
            ++currentLine_;
            currentColumn_ = 1;
        } else {
            ++currentColumn_;
        }
        ++currentPosition_;
    }
}

Token Lexer::readIdentifier() {
    size_t start = currentPosition_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (!isEOF() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        ++currentPosition_;
        ++currentColumn_;
    }
    
    std::string value = source_.substr(start, currentPosition_ - start);
    
    // 检查是否为关键字
    if (isKeyword(value)) {
        if (value == "text") return createToken(TokenType::TEXT, value);
        if (value == "script") return createToken(TokenType::SCRIPT, value);
        if (value == "inherit") return createToken(TokenType::INHERIT, value);
        if (value == "delete") return createToken(TokenType::DELETE, value);
        if (value == "insert") return createToken(TokenType::INSERT, value);
        if (value == "after") return createToken(TokenType::AFTER, value);
        if (value == "before") return createToken(TokenType::BEFORE, value);
        if (value == "replace") return createToken(TokenType::REPLACE, value);
        if (value == "from") return createToken(TokenType::FROM, value);
        if (value == "as") return createToken(TokenType::AS, value);
        if (value == "except") return createToken(TokenType::EXCEPT, value);
        if (value == "use") return createToken(TokenType::USE, value);
        if (value == "html5") return createToken(TokenType::HTML5, value);
    }
    
    return createToken(TokenType::IDENTIFIER, value);
}

Token Lexer::readString() {
    char quote = currentChar();
    size_t start = currentPosition_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    ++currentPosition_;
    ++currentColumn_;
    
    while (!isEOF() && currentChar() != quote) {
        if (currentChar() == '\\') {
            ++currentPosition_;
            ++currentColumn_;
        }
        if (currentChar() == '\n') {
            ++currentLine_;
            currentColumn_ = 1;
        } else {
            ++currentColumn_;
        }
        ++currentPosition_;
    }
    
    if (!isEOF()) {
        ++currentPosition_;
        ++currentColumn_;
    }
    
    std::string value = source_.substr(start, currentPosition_ - start);
    return createToken(TokenType::STRING, value);
}

Token Lexer::readNumber() {
    size_t start = currentPosition_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (!isEOF() && (isDigit(currentChar()) || currentChar() == '.')) {
        ++currentPosition_;
        ++currentColumn_;
    }
    
    std::string value = source_.substr(start, currentPosition_ - start);
    return createToken(TokenType::NUMBER, value);
}

Token Lexer::readSymbol() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    switch (c) {
        case ':':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::COLON, ":");
        case '=':
            ++currentPosition_;
            ++currentColumn_;
            if (currentChar() == '=') {
                ++currentPosition_;
                ++currentColumn_;
                return createToken(TokenType::EQUAL, "==");
            }
            return createToken(TokenType::EQUALS, "=");
        case ';':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::SEMICOLON, ";");
        case ',':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::COMMA, ",");
        case '{':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::LBRACE, "{");
        case '}':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::RBRACE, "}");
        case '[':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::LBRACKET, "[");
        case ']':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::RBRACKET, "]");
        case '(':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::LPAREN, "(");
        case ')':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::RPAREN, ")");
        case '@':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::AT_SYMBOL, "@");
        case '#':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::HASH, "#");
        case '.':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::DOT, ".");
        case '&':
            ++currentPosition_;
            ++currentColumn_;
            if (currentChar() == '&') {
                ++currentPosition_;
                ++currentColumn_;
                return createToken(TokenType::AND, "&&");
            }
            return createToken(TokenType::AMPERSAND, "&");
        case '?':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::QUESTION, "?");
        case '!':
            ++currentPosition_;
            ++currentColumn_;
            if (currentChar() == '=') {
                ++currentPosition_;
                ++currentColumn_;
                return createToken(TokenType::NOT_EQUAL, "!=");
            }
            return createToken(TokenType::NOT, "!");
        case '_':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::UNDERSCORE, "_");
        case '<':
            ++currentPosition_;
            ++currentColumn_;
            if (currentChar() == '=') {
                ++currentPosition_;
                ++currentColumn_;
                return createToken(TokenType::LESS_EQUAL, "<=");
            }
            return createToken(TokenType::LESS, "<");
        case '>':
            ++currentPosition_;
            ++currentColumn_;
            if (currentChar() == '=') {
                ++currentPosition_;
                ++currentColumn_;
                return createToken(TokenType::GREATER_EQUAL, ">=");
            }
            return createToken(TokenType::GREATER, ">");
        case '+':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::PLUS, "+");
        case '-':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::MINUS, "-");
        case '*':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::MULTIPLY, "*");
        case '/':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::DIVIDE, "/");
        case '%':
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::MODULO, "%");
        case '|':
            ++currentPosition_;
            ++currentColumn_;
            if (currentChar() == '|') {
                ++currentPosition_;
                ++currentColumn_;
                return createToken(TokenType::OR, "||");
            }
            return createToken(TokenType::UNKNOWN, "|");
        default:
            ++currentPosition_;
            ++currentColumn_;
            return createToken(TokenType::UNKNOWN, std::string(1, c));
    }
}

Token Lexer::readGeneratorComment() {
    char c = currentChar();
    size_t start = currentPosition_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    if (c == '/' && nextChar() == '/') {
        skipLineComment();
    } else if (c == '/' && nextChar() == '*') {
        skipBlockComment();
    } else if (c == '-' && nextChar() == '-') {
        skipLineComment();
    }
    
    std::string value = source_.substr(start, currentPosition_ - start);
    return createToken(TokenType::GENERATOR_COMMENT, value);
}

bool Lexer::isKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "text", "script", "inherit", "delete", "insert", "after", "before",
        "replace", "from", "as", "except", "use", "html5"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool Lexer::isTypeKeyword(const std::string& word) const {
    static const std::vector<std::string> typeKeywords = {
        "Style", "Element", "Var", "Html", "JavaScript", "Chtl", "CJmod"
    };
    
    return std::find(typeKeywords.begin(), typeKeywords.end(), word) != typeKeywords.end();
}

bool Lexer::isOperatorKeyword(const std::string& word) const {
    static const std::vector<std::string> operatorKeywords = {
        "and", "or", "not", "eq", "ne", "lt", "le", "gt", "ge"
    };
    
    return std::find(operatorKeywords.begin(), operatorKeywords.end(), word) != operatorKeywords.end();
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char Lexer::currentChar() const {
    if (isEOF()) return '\0';
    return source_[currentPosition_];
}

char Lexer::nextChar() {
    if (isEOF()) return '\0';
    ++currentPosition_;
    if (currentPosition_ < source_.length()) {
        if (source_[currentPosition_ - 1] == '\n') {
            ++currentLine_;
            currentColumn_ = 1;
        } else {
            ++currentColumn_;
        }
        return source_[currentPosition_];
    }
    return '\0';
}

void Lexer::ungetChar() {
    if (currentPosition_ > 0) {
        --currentPosition_;
        if (source_[currentPosition_] == '\n') {
            --currentLine_;
            // 需要重新计算列号
            size_t pos = currentPosition_;
            while (pos > 0 && source_[pos - 1] != '\n') {
                --pos;
            }
            currentColumn_ = currentPosition_ - pos + 1;
        } else {
            --currentColumn_;
        }
    }
}

Token Lexer::createToken(TokenType type, const std::string& value) {
    return Token(type, value, currentLine_, currentColumn_, currentPosition_);
}

} // namespace CHTL