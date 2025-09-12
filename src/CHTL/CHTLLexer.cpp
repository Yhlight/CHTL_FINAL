#include "CHTL/CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <sstream>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : position_(0), line_(1), column_(1), debug_mode_(false), 
      skip_whitespace_(true), skip_comments_(false) {
    initializeKeywords();
}

CHTLLexer::~CHTLLexer() = default;

void CHTLLexer::setInput(const std::string& input) {
    input_ = input;
    position_ = 0;
    line_ = 1;
    column_ = 1;
}

void CHTLLexer::setInput(std::istream& input) {
    std::ostringstream oss;
    oss << input.rdbuf();
    setInput(oss.str());
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    reset();
    
    while (hasMoreTokens()) {
        Token token = nextToken();
        if (token.type != TokenType::EOF_TOKEN) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

Token CHTLLexer::nextToken() {
    if (!hasMoreTokens()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_, position_);
    }
    
    // 跳过空白字符
    if (skip_whitespace_) {
        skipWhitespace();
    }
    
    if (!hasMoreTokens()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_, position_);
    }
    
    char c = currentChar();
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return scanString();
    }
    
    // 注释
    if (c == '/' && peekChar() == '/') {
        return scanComment();
    }
    
    if (c == '/' && peekChar() == '*') {
        return scanComment();
    }
    
    if (c == '-' && peekChar() == '-') {
        return scanComment();
    }
    
    // 操作符
    if (isOperatorChar(c)) {
        return scanOperator();
    }
    
    // 无修饰字面量（特殊处理）
    if (c != '{' && c != '}' && c != '[' && c != ']' && 
        c != '(' && c != ')' && c != ';' && c != ':' && 
        c != '=' && c != ',' && c != '.' && c != ' ' && 
        c != '\t' && c != '\n' && c != '\r') {
        return scanUnquotedLiteral();
    }
    
    // 其他字符
    Token token(TokenType::UNKNOWN, std::string(1, c), line_, column_, position_);
    nextChar();
    return token;
}

Token CHTLLexer::peekToken() {
    size_t old_position = position_;
    size_t old_line = line_;
    size_t old_column = column_;
    
    Token token = nextToken();
    
    position_ = old_position;
    line_ = old_line;
    column_ = old_column;
    
    return token;
}

void CHTLLexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
}

void CHTLLexer::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}

void CHTLLexer::setSkipWhitespace(bool skip) {
    skip_whitespace_ = skip;
}

void CHTLLexer::setSkipComments(bool skip) {
    skip_comments_ = skip;
}

bool CHTLLexer::hasMoreTokens() const {
    return position_ < input_.length();
}

size_t CHTLLexer::getCurrentLine() const {
    return line_;
}

size_t CHTLLexer::getCurrentColumn() const {
    return column_;
}

size_t CHTLLexer::getCurrentPosition() const {
    return position_;
}

void CHTLLexer::initializeKeywords() {
    // HTML元素关键字
    keywords_["html"] = TokenType::KEYWORD_HTML;
    keywords_["head"] = TokenType::KEYWORD_HEAD;
    keywords_["body"] = TokenType::KEYWORD_BODY;
    keywords_["div"] = TokenType::KEYWORD_DIV;
    keywords_["span"] = TokenType::KEYWORD_SPAN;
    keywords_["p"] = TokenType::KEYWORD_P;
    keywords_["h1"] = TokenType::KEYWORD_H1;
    keywords_["h2"] = TokenType::KEYWORD_H2;
    keywords_["h3"] = TokenType::KEYWORD_H3;
    keywords_["h4"] = TokenType::KEYWORD_H4;
    keywords_["h5"] = TokenType::KEYWORD_H5;
    keywords_["h6"] = TokenType::KEYWORD_H6;
    keywords_["title"] = TokenType::KEYWORD_TITLE;
    keywords_["script"] = TokenType::KEYWORD_SCRIPT;
    keywords_["style"] = TokenType::KEYWORD_STYLE;
    keywords_["text"] = TokenType::KEYWORD_TEXT;
    
    // CHTL关键字
    keywords_["[Template]"] = TokenType::KEYWORD_TEMPLATE;
    keywords_["[Custom]"] = TokenType::KEYWORD_CUSTOM;
    keywords_["[Origin]"] = TokenType::KEYWORD_ORIGIN;
    keywords_["[Import]"] = TokenType::KEYWORD_IMPORT;
    keywords_["[Namespace]"] = TokenType::KEYWORD_NAMESPACE;
    keywords_["[Configuration]"] = TokenType::KEYWORD_CONFIGURATION;
    
    // 模板类型关键字
    keywords_["@Style"] = TokenType::KEYWORD_STYLE;
    keywords_["@Element"] = TokenType::KEYWORD_ELEMENT;
    keywords_["@Var"] = TokenType::KEYWORD_VAR;
    
    // 特例化操作关键字
    keywords_["delete"] = TokenType::KEYWORD_DELETE;
    keywords_["insert"] = TokenType::KEYWORD_INSERT;
    keywords_["after"] = TokenType::KEYWORD_AFTER;
    keywords_["before"] = TokenType::KEYWORD_BEFORE;
    keywords_["replace"] = TokenType::KEYWORD_REPLACE;
    keywords_["at"] = TokenType::KEYWORD_AT;
    keywords_["top"] = TokenType::KEYWORD_TOP;
    keywords_["bottom"] = TokenType::KEYWORD_BOTTOM;
}

char CHTLLexer::currentChar() const {
    if (position_ >= input_.length()) {
        return '\0';
    }
    return input_[position_];
}

char CHTLLexer::peekChar() const {
    if (position_ + 1 >= input_.length()) {
        return '\0';
    }
    return input_[position_ + 1];
}

char CHTLLexer::nextChar() {
    if (position_ >= input_.length()) {
        return '\0';
    }
    
    char c = input_[position_++];
    
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    return c;
}

void CHTLLexer::skipWhitespace() {
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        nextChar();
    }
}

void CHTLLexer::skipLine() {
    while (hasMoreTokens() && !isNewline(currentChar())) {
        nextChar();
    }
    if (hasMoreTokens()) {
        nextChar(); // 跳过换行符
    }
}

Token CHTLLexer::scanIdentifier() {
    size_t start_pos = position_;
    size_t start_line = line_;
    size_t start_column = column_;
    
    while (hasMoreTokens() && isAlphaNumeric(currentChar())) {
        nextChar();
    }
    
    std::string value = input_.substr(start_pos, position_ - start_pos);
    
    // 检查是否是关键字
    auto it = keywords_.find(value);
    if (it != keywords_.end()) {
        return Token(it->second, value, start_line, start_column, start_pos);
    }
    
    return Token(TokenType::IDENTIFIER, value, start_line, start_column, start_pos);
}

Token CHTLLexer::scanString() {
    size_t start_pos = position_;
    size_t start_line = line_;
    size_t start_column = column_;
    
    char quote = currentChar();
    nextChar(); // 跳过开始引号
    
    std::string value;
    while (hasMoreTokens() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar() == quote) {
            nextChar(); // 跳过反斜杠
            value += currentChar();
            nextChar();
        } else {
            value += currentChar();
            nextChar();
        }
    }
    
    if (hasMoreTokens()) {
        nextChar(); // 跳过结束引号
    }
    
    return Token(TokenType::STRING_LITERAL, value, start_line, start_column, start_pos);
}

Token CHTLLexer::scanNumber() {
    size_t start_pos = position_;
    size_t start_line = line_;
    size_t start_column = column_;
    
    std::string value;
    
    // 整数部分
    while (hasMoreTokens() && isDigit(currentChar())) {
        value += currentChar();
        nextChar();
    }
    
    // 小数部分
    if (hasMoreTokens() && currentChar() == '.') {
        value += currentChar();
        nextChar();
        
        while (hasMoreTokens() && isDigit(currentChar())) {
            value += currentChar();
            nextChar();
        }
    }
    
    // 科学计数法
    if (hasMoreTokens() && (currentChar() == 'e' || currentChar() == 'E')) {
        value += currentChar();
        nextChar();
        
        if (hasMoreTokens() && (currentChar() == '+' || currentChar() == '-')) {
            value += currentChar();
            nextChar();
        }
        
        while (hasMoreTokens() && isDigit(currentChar())) {
            value += currentChar();
            nextChar();
        }
    }
    
    return Token(TokenType::NUMBER_LITERAL, value, start_line, start_column, start_pos);
}

Token CHTLLexer::scanOperator() {
    size_t start_pos = position_;
    size_t start_line = line_;
    size_t start_column = column_;
    
    char c = currentChar();
    nextChar();
    
    // 双字符操作符
    if (hasMoreTokens()) {
        char next = currentChar();
        
        switch (c) {
            case '=':
                if (next == '=') {
                    nextChar();
                    return Token(TokenType::EQUAL_EQUAL, "==", start_line, start_column, start_pos);
                }
                break;
            case '!':
                if (next == '=') {
                    nextChar();
                    return Token(TokenType::NOT_EQUAL, "!=", start_line, start_column, start_pos);
                }
                break;
            case '<':
                if (next == '=') {
                    nextChar();
                    return Token(TokenType::LESS_EQUAL, "<=", start_line, start_column, start_pos);
                }
                break;
            case '>':
                if (next == '=') {
                    nextChar();
                    return Token(TokenType::GREATER_EQUAL, ">=", start_line, start_column, start_pos);
                }
                break;
            case '&':
                if (next == '&') {
                    nextChar();
                    return Token(TokenType::AND, "&&", start_line, start_column, start_pos);
                }
                break;
            case '|':
                if (next == '|') {
                    nextChar();
                    return Token(TokenType::OR, "||", start_line, start_column, start_pos);
                }
                break;
            case '*':
                if (next == '*') {
                    nextChar();
                    return Token(TokenType::POWER, "**", start_line, start_column, start_pos);
                }
                break;
            case '-':
                if (next == '>') {
                    nextChar();
                    return Token(TokenType::ARROW, "->", start_line, start_column, start_pos);
                }
                break;
        }
    }
    
    // 单字符操作符
    switch (c) {
        case '{': return Token(TokenType::LEFT_BRACE, "{", start_line, start_column, start_pos);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", start_line, start_column, start_pos);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", start_line, start_column, start_pos);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", start_line, start_column, start_pos);
        case '(': return Token(TokenType::LEFT_PAREN, "(", start_line, start_column, start_pos);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", start_line, start_column, start_pos);
        case ';': return Token(TokenType::SEMICOLON, ";", start_line, start_column, start_pos);
        case ':': return Token(TokenType::COLON, ":", start_line, start_column, start_pos);
        case '=': return Token(TokenType::EQUALS, "=", start_line, start_column, start_pos);
        case ',': return Token(TokenType::COMMA, ",", start_line, start_column, start_pos);
        case '.': return Token(TokenType::DOT, ".", start_line, start_column, start_pos);
        case '?': return Token(TokenType::QUESTION, "?", start_line, start_column, start_pos);
        case '!': return Token(TokenType::EXCLAMATION, "!", start_line, start_column, start_pos);
        case '&': return Token(TokenType::AMPERSAND, "&", start_line, start_column, start_pos);
        case '|': return Token(TokenType::PIPE, "|", start_line, start_column, start_pos);
        case '~': return Token(TokenType::TILDE, "~", start_line, start_column, start_pos);
        case '^': return Token(TokenType::CARET, "^", start_line, start_column, start_pos);
        case '$': return Token(TokenType::DOLLAR, "$", start_line, start_column, start_pos);
        case '@': return Token(TokenType::AT, "@", start_line, start_column, start_pos);
        case '#': return Token(TokenType::HASH, "#", start_line, start_column, start_pos);
        case '+': return Token(TokenType::PLUS, "+", start_line, start_column, start_pos);
        case '-': return Token(TokenType::MINUS, "-", start_line, start_column, start_pos);
        case '*': return Token(TokenType::MULTIPLY, "*", start_line, start_column, start_pos);
        case '/': return Token(TokenType::DIVIDE, "/", start_line, start_column, start_pos);
        case '%': return Token(TokenType::MODULO, "%", start_line, start_column, start_pos);
        case '<': return Token(TokenType::LESS_THAN, "<", start_line, start_column, start_pos);
        case '>': return Token(TokenType::GREATER_THAN, ">", start_line, start_column, start_pos);
        default: return Token(TokenType::UNKNOWN, std::string(1, c), start_line, start_column, start_pos);
    }
}

Token CHTLLexer::scanComment() {
    size_t start_pos = position_;
    size_t start_line = line_;
    size_t start_column = column_;
    
    char c = currentChar();
    nextChar();
    
    if (c == '/' && currentChar() == '/') {
        // 单行注释
        nextChar();
        skipLine();
        return Token(TokenType::SINGLE_LINE_COMMENT, "", start_line, start_column, start_pos);
    } else if (c == '/' && currentChar() == '*') {
        // 多行注释
        nextChar();
        while (hasMoreTokens()) {
            if (currentChar() == '*' && peekChar() == '/') {
                nextChar();
                nextChar();
                break;
            }
            nextChar();
        }
        return Token(TokenType::MULTI_LINE_COMMENT, "", start_line, start_column, start_pos);
    } else if (c == '-' && currentChar() == '-') {
        // 生成器注释
        nextChar();
        skipLine();
        return Token(TokenType::GENERATOR_COMMENT, "", start_line, start_column, start_pos);
    }
    
    return Token(TokenType::UNKNOWN, std::string(1, c), start_line, start_column, start_pos);
}

Token CHTLLexer::scanUnquotedLiteral() {
    size_t start_pos = position_;
    size_t start_line = line_;
    size_t start_column = column_;
    
    std::string value;
    
    while (hasMoreTokens() && !isWhitespace(currentChar()) && 
           !isOperatorChar(currentChar()) && currentChar() != '\0') {
        value += currentChar();
        nextChar();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, start_line, start_column, start_pos);
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

bool CHTLLexer::isOperatorChar(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' || 
           c == '(' || c == ')' || c == ';' || c == ':' || 
           c == '=' || c == ',' || c == '.' || c == '?' || 
           c == '!' || c == '&' || c == '|' || c == '~' || 
           c == '^' || c == '$' || c == '@' || c == '#' || 
           c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '%' || c == '<' || c == '>';
}

void CHTLLexer::reportError(const std::string& message) {
    if (debug_mode_) {
        std::cerr << "[CHTLLexer] Error at line " << line_ << ", column " << column_ 
                  << ": " << message << std::endl;
    }
}

Token CHTLLexer::createErrorToken(const std::string& message) {
    reportError(message);
    return Token(TokenType::UNKNOWN, message, line_, column_, position_);
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << static_cast<int>(type) << ", \"" << value << "\", " 
        << line_ << ":" << column_ << ")";
    return oss.str();
}

} // namespace CHTL