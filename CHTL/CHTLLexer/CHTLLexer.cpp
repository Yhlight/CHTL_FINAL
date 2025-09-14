#include "CHTLLexer.h"
#include <iostream>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : position_(0)
    , line_(1)
    , column_(1)
    , debugMode_(false)
    , caseSensitive_(true) {
}

std::vector<Token> CHTLLexer::tokenize(const std::string& sourceCode) {
    source_ = sourceCode;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    errors_.clear();
    
    std::vector<Token> tokens;
    
    while (position_ < source_.length()) {
        Token token = getNextToken();
        if (token.type != TokenType::WHITESPACE) {
            tokens.push_back(token);
        }
        
        if (token.type == TokenType::ERROR) {
            if (debugMode_) {
                std::cerr << "Lexer error at line " << token.line 
                          << ", column " << token.column << ": " << token.value << std::endl;
            }
        }
    }
    
    // 添加EOF标记
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line_, column_, position_);
    
    return tokens;
}

Token CHTLLexer::getNextToken() {
    skipWhitespace();
    
    if (position_ >= source_.length()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_, position_);
    }
    
    char c = currentChar();
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    // 注释处理
    if (isCommentStart(c)) {
        return readComment();
    }
    
    // 生成器注释处理
    if (isGeneratorCommentStart(c)) {
        return readGeneratorComment();
    }
    
    // 字符串处理
    if (isStringDelimiter(c)) {
        return readString();
    }
    
    // 数字处理
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 标识符和关键字处理
    if (isLetter(c)) {
        return readIdentifier();
    }
    
    // 符号处理
    if (isSymbol(c)) {
        return readSymbol();
    }
    
    // 未知字符
    addError("Unknown character: " + std::string(1, c), startLine, startColumn);
    advance();
    return Token(TokenType::ERROR, std::string(1, c), startLine, startColumn, startPos);
}

Token CHTLLexer::readIdentifier() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string value;
    while (position_ < source_.length() && isAlphaNumeric(currentChar())) {
        value += advance();
    }
    
    // 检查是否为关键字
    TokenType type = KeywordMap::getKeywordType(value);
    if (type == TokenType::IDENTIFIER) {
        // 检查是否为字面量（无修饰字符串）
        if (isLiteral(value)) {
            type = TokenType::LITERAL;
        }
    }
    
    return Token(type, value, startLine, startColumn, startPos);
}

Token CHTLLexer::readString() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    char delimiter = advance(); // 跳过开始引号
    std::string value;
    
    while (position_ < source_.length() && currentChar() != delimiter) {
        if (currentChar() == '\\' && position_ + 1 < source_.length()) {
            // 处理转义字符
            advance(); // 跳过反斜杠
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += escaped; break;
            }
        } else {
            value += advance();
        }
    }
    
    if (position_ < source_.length()) {
        advance(); // 跳过结束引号
    } else {
        addError("Unterminated string", startLine, startColumn);
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn, startPos);
}

Token CHTLLexer::readNumber() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string value;
    
    // 读取整数部分
    while (position_ < source_.length() && isDigit(currentChar())) {
        value += advance();
    }
    
    // 读取小数部分
    if (position_ < source_.length() && currentChar() == '.') {
        value += advance();
        while (position_ < source_.length() && isDigit(currentChar())) {
            value += advance();
        }
    }
    
    // 读取科学计数法
    if (position_ < source_.length() && (currentChar() == 'e' || currentChar() == 'E')) {
        value += advance();
        if (position_ < source_.length() && (currentChar() == '+' || currentChar() == '-')) {
            value += advance();
        }
        while (position_ < source_.length() && isDigit(currentChar())) {
            value += advance();
        }
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, startPos);
}

Token CHTLLexer::readLiteral() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string value;
    
    // 读取字面量直到遇到特殊字符
    while (position_ < source_.length() && 
           !isWhitespace(currentChar()) && 
           !isSymbol(currentChar()) && 
           currentChar() != '{' && 
           currentChar() != '}' &&
           currentChar() != ';' &&
           currentChar() != ':') {
        value += advance();
    }
    
    return Token(TokenType::LITERAL, value, startLine, startColumn, startPos);
}

Token CHTLLexer::readComment() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string value;
    
    // 单行注释 //
    if (position_ + 1 < source_.length() && source_[position_ + 1] == '/') {
        advance(); // 跳过第一个 /
        advance(); // 跳过第二个 /
        
        while (position_ < source_.length() && currentChar() != '\n') {
            value += advance();
        }
        
        return Token(TokenType::COMMENT, value, startLine, startColumn, startPos);
    }
    
    // 多行注释 /* */
    if (position_ + 1 < source_.length() && source_[position_ + 1] == '*') {
        advance(); // 跳过 /
        advance(); // 跳过 *
        
        while (position_ < source_.length()) {
            if (position_ + 1 < source_.length() && 
                currentChar() == '*' && source_[position_ + 1] == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                break;
            }
            value += advance();
        }
        
        return Token(TokenType::MULTILINE_COMMENT, value, startLine, startColumn, startPos);
    }
    
    // 不是注释，回退
    position_ = startPos;
    line_ = startLine;
    column_ = startColumn;
    return readSymbol();
}

Token CHTLLexer::readGeneratorComment() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string value;
    
    advance(); // 跳过 #
    
    // 检查是否有空格
    if (position_ < source_.length() && currentChar() == ' ') {
        advance(); // 跳过空格
    }
    
    while (position_ < source_.length() && currentChar() != '\n') {
        value += advance();
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, startPos);
}

Token CHTLLexer::readSymbol() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    char c = advance();
    std::string value(1, c);
    
    // 处理双字符符号
    if (position_ < source_.length()) {
        char next = currentChar();
        std::string twoChar = value + next;
        
        // 检查是否为双字符关键字
        if (twoChar == "//" || twoChar == "/*" || twoChar == "*/" || 
            twoChar == "==" || twoChar == "!=" || twoChar == "<=" || 
            twoChar == ">=" || twoChar == "&&" || twoChar == "||" ||
            twoChar == "++" || twoChar == "--" || twoChar == "+=" ||
            twoChar == "-=" || twoChar == "*=" || twoChar == "/=" ||
            twoChar == "%=" || twoChar == "&=" || twoChar == "|=" ||
            twoChar == "^=" || twoChar == "<<=" || twoChar == ">>=") {
            advance();
            value = twoChar;
        }
    }
    
    // 映射到TokenType
    TokenType type = TokenType::ERROR;
    if (value == "{") type = TokenType::LEFT_BRACE;
    else if (value == "}") type = TokenType::RIGHT_BRACE;
    else if (value == "[") type = TokenType::LEFT_BRACKET;
    else if (value == "]") type = TokenType::RIGHT_BRACKET;
    else if (value == "(") type = TokenType::LEFT_PAREN;
    else if (value == ")") type = TokenType::RIGHT_PAREN;
    else if (value == ";") type = TokenType::SEMICOLON;
    else if (value == ":") type = TokenType::COLON;
    else if (value == "=") type = TokenType::EQUALS;
    else if (value == ",") type = TokenType::COMMA;
    else if (value == ".") type = TokenType::DOT;
    else if (value == "#") type = TokenType::HASH;
    else if (value == "@") type = TokenType::AT;
    else if (value == "&") type = TokenType::AMPERSAND;
    else if (value == "?") type = TokenType::QUESTION;
    else if (value == "!") type = TokenType::EXCLAMATION;
    else if (value == "|") type = TokenType::PIPE;
    else if (value == "~") type = TokenType::TILDE;
    else if (value == "^") type = TokenType::CARET;
    else if (value == "$") type = TokenType::DOLLAR;
    else if (value == "%") type = TokenType::PERCENT;
    else if (value == "+") type = TokenType::PLUS;
    else if (value == "-") type = TokenType::MINUS;
    else if (value == "*") type = TokenType::ASTERISK;
    else if (value == "/") type = TokenType::SLASH;
    else if (value == "\\") type = TokenType::BACKSLASH;
    else if (value == "<") type = TokenType::LESS_THAN;
    else if (value == ">") type = TokenType::GREATER_THAN;
    
    return Token(type, value, startLine, startColumn, startPos);
}

char CHTLLexer::currentChar() const {
    if (position_ >= source_.length()) {
        return '\0';
    }
    return source_[position_];
}

char CHTLLexer::peekChar() const {
    if (position_ + 1 >= source_.length()) {
        return '\0';
    }
    return source_[position_ + 1];
}

char CHTLLexer::advance() {
    if (position_ >= source_.length()) {
        return '\0';
    }
    
    char c = source_[position_];
    position_++;
    
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    return c;
}

void CHTLLexer::skipWhitespace() {
    while (position_ < source_.length() && isWhitespace(currentChar())) {
        advance();
    }
}

bool CHTLLexer::isLetter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isLetter(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLLexer::isSymbol(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' ||
           c == ';' || c == ':' || c == '=' || c == ',' || c == '.' || c == '#' ||
           c == '@' || c == '&' || c == '?' || c == '!' || c == '|' || c == '~' ||
           c == '^' || c == '$' || c == '%' || c == '+' || c == '-' || c == '*' ||
           c == '/' || c == '\\' || c == '<' || c == '>';
}

bool CHTLLexer::isStringDelimiter(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isCommentStart(char c) const {
    return c == '/';
}

bool CHTLLexer::isGeneratorCommentStart(char c) const {
    return c == '#';
}

bool CHTLLexer::isLiteral(const std::string& value) const {
    // 简单的字面量检测：不包含特殊字符的标识符
    for (char c : value) {
        if (isSymbol(c) || c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            return false;
        }
    }
    return true;
}

void CHTLLexer::addError(const std::string& message) {
    errors_.push_back(message);
}

void CHTLLexer::addError(const std::string& message, size_t line, size_t column) {
    errors_.push_back("Line " + std::to_string(line) + ", Column " + std::to_string(column) + ": " + message);
}

void CHTLLexer::debugToken(const Token& token) const {
    if (debugMode_) {
        std::cout << "Token: " << token.toString() << std::endl;
    }
}

} // namespace CHTL