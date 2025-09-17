#include "CHTL/CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {

// 关键字映射表
const std::unordered_map<String, TokenType> CHTLLexer::keywords_ = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"template", TokenType::KEYWORD_TEMPLATE},
    {"custom", TokenType::KEYWORD_CUSTOM},
    {"import", TokenType::KEYWORD_IMPORT},
    {"namespace", TokenType::KEYWORD_NAMESPACE},
    {"configuration", TokenType::KEYWORD_CONFIGURATION},
    {"origin", TokenType::KEYWORD_ORIGIN},
    {"use", TokenType::KEYWORD_USE},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT_TOP},
    {"top", TokenType::KEYWORD_AT_TOP},
    {"bottom", TokenType::KEYWORD_AT_BOTTOM},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"html5", TokenType::KEYWORD_HTML5}
};

const std::unordered_map<String, TokenType> CHTLLexer::templateKeywords_ = {
    {"@Style", TokenType::TEMPLATE_STYLE},
    {"@Element", TokenType::TEMPLATE_ELEMENT},
    {"@Var", TokenType::TEMPLATE_VAR}
};

const std::unordered_map<String, TokenType> CHTLLexer::customKeywords_ = {
    {"[Custom]", TokenType::CUSTOM_STYLE},
    {"[Custom] @Style", TokenType::CUSTOM_STYLE},
    {"[Custom] @Element", TokenType::CUSTOM_ELEMENT},
    {"[Custom] @Var", TokenType::CUSTOM_VAR}
};

const std::unordered_map<String, TokenType> CHTLLexer::importKeywords_ = {
    {"[Import]", TokenType::IMPORT_HTML},
    {"[Import] @Html", TokenType::IMPORT_HTML},
    {"[Import] @Style", TokenType::IMPORT_STYLE},
    {"[Import] @JavaScript", TokenType::IMPORT_JAVASCRIPT},
    {"[Import] @Chtl", TokenType::IMPORT_CHTL},
    {"[Import] @CJmod", TokenType::IMPORT_CJMOD}
};

const std::unordered_map<String, TokenType> CHTLLexer::originKeywords_ = {
    {"[Origin]", TokenType::ORIGIN_HTML},
    {"[Origin] @Html", TokenType::ORIGIN_HTML},
    {"[Origin] @Style", TokenType::ORIGIN_STYLE},
    {"[Origin] @JavaScript", TokenType::ORIGIN_JAVASCRIPT}
};

const std::unordered_map<String, TokenType> CHTLLexer::configurationKeywords_ = {
    {"[Configuration]", TokenType::CONFIGURATION},
    {"@Config", TokenType::CONFIG},
    {"[Info]", TokenType::INFO},
    {"[Export]", TokenType::EXPORT}
};

const std::unordered_map<String, TokenType> CHTLLexer::operatorKeywords_ = {
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::MULTIPLY},
    {"/", TokenType::DIVIDE},
    {"%", TokenType::MODULO},
    {"**", TokenType::POWER},
    {"<", TokenType::LESS_THAN},
    {">", TokenType::GREATER_THAN},
    {"<=", TokenType::LESS_EQUAL},
    {">=", TokenType::GREATER_EQUAL},
    {"==", TokenType::EQUAL},
    {"!=", TokenType::NOT_EQUAL},
    {"&&", TokenType::LOGICAL_AND},
    {"||", TokenType::LOGICAL_OR},
    {"!", TokenType::NOT}
};

// 构造函数
CHTLLexer::CHTLLexer(const StringView& source)
    : source_(source), position_(0), line_(1), column_(1) {
}

// 主要tokenize方法
TokenList CHTLLexer::tokenize() {
    tokens_.clear();
    errors_.clear();
    position_ = 0;
    line_ = 1;
    column_ = 1;
    
    while (!isAtEnd()) {
        Token token = scanToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens_.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens_.emplace_back(TokenType::EOF_TOKEN, "", SourceLocation(getCurrentPosition(), getCurrentPosition()));
    
    return tokens_;
}

// 扫描单个token
Token CHTLLexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", SourceLocation(getCurrentPosition(), getCurrentPosition()));
    }
    
    char c = current();
    
    // 标识符和关键字
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 字符串字面量
    if (isQuote(c)) {
        return scanString();
    }
    
    // 数字字面量
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 注释
    if (c == '/' && peek(1) == '/') {
        return scanLineComment();
    }
    if (c == '/' && peek(1) == '*') {
        return scanBlockComment();
    }
    if (c == '#') {
        return scanGeneratorComment();
    }
    
    // 操作符
    if (isOperator(c)) {
        return scanOperator();
    }
    
    // 分隔符
    if (isDelimiter(c)) {
        return scanDelimiter();
    }
    
    // 特殊字符
    if (isSpecial(c)) {
        return scanSpecial();
    }
    
    // 未知字符
    addError("Unknown character: " + String(1, c), getCurrentPosition());
    advance();
    return Token(TokenType::UNKNOWN, String(1, c), SourceLocation(getCurrentPosition(), getCurrentPosition()));
}

// 字符检查方法
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

bool CHTLLexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isDelimiter(char c) const {
    return c == '{' || c == '}' || c == '(' || c == ')' || 
           c == '[' || c == ']' || c == ';' || c == ',' || 
           c == '.' || c == ':' || c == '=';
}

bool CHTLLexer::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '<' || c == '>' || c == '!' || c == '&' || c == '|';
}

bool CHTLLexer::isSpecial(char c) const {
    return c == '@' || c == '$' || c == '#' || c == '?' || c == '~' || c == '^';
}

// 位置管理方法
Position CHTLLexer::getCurrentPosition() const {
    return Position(line_, column_, position_);
}

void CHTLLexer::advance() {
    if (!isAtEnd()) {
        if (isNewline(current())) {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLLexer::advance(int count) {
    for (int i = 0; i < count; i++) {
        advance();
    }
}

char CHTLLexer::peek(int offset) const {
    size_t peekPos = position_ + offset;
    if (peekPos >= source_.length()) {
        return '\0';
    }
    return source_[peekPos];
}

char CHTLLexer::current() const {
    return peek(0);
}

bool CHTLLexer::isAtEnd() const {
    return position_ >= source_.length();
}

// 扫描标识符
Token CHTLLexer::scanIdentifier() {
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    while (!isAtEnd() && isAlphaNumeric(current())) {
        advance();
    }
    
    StringView value = source_.substr(start, position_ - start);
    String identifier(value);
    
    // 检查是否为关键字
    TokenType type = TokenType::IDENTIFIER;
    if (keywords_.find(identifier) != keywords_.end()) {
        type = keywords_.at(identifier);
    }
    
    return Token(type, value, SourceLocation(startPos, getCurrentPosition()));
}

// 扫描字符串
Token CHTLLexer::scanString() {
    char quote = current();
    advance(); // 跳过开始引号
    
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\') {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    if (isAtEnd()) {
        addError("Unterminated string literal", getCurrentPosition());
        return Token(TokenType::UNKNOWN, "", SourceLocation(getCurrentPosition(), getCurrentPosition()));
    }
    
    StringView value = source_.substr(start, position_ - start);
    advance(); // 跳过结束引号
    
    return Token(TokenType::STRING_LITERAL, value, SourceLocation(startPos, getCurrentPosition()));
}

// 扫描数字
Token CHTLLexer::scanNumber() {
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    // 扫描整数部分
    while (!isAtEnd() && isDigit(current())) {
        advance();
    }
    
    // 检查是否有小数点
    if (!isAtEnd() && current() == '.' && isDigit(peek(1))) {
        advance(); // 跳过小数点
        while (!isAtEnd() && isDigit(current())) {
            advance();
        }
    }
    
    // 检查是否有指数
    if (!isAtEnd() && (current() == 'e' || current() == 'E')) {
        advance(); // 跳过e或E
        if (!isAtEnd() && (current() == '+' || current() == '-')) {
            advance(); // 跳过符号
        }
        while (!isAtEnd() && isDigit(current())) {
            advance();
        }
    }
    
    StringView value = source_.substr(start, position_ - start);
    return Token(TokenType::NUMBER_LITERAL, value, SourceLocation(startPos, getCurrentPosition()));
}

// 扫描注释
Token CHTLLexer::scanLineComment() {
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    advance(2); // 跳过 //
    
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
    
    StringView value = source_.substr(start, position_ - start);
    return Token(TokenType::LINE_COMMENT, value, SourceLocation(startPos, getCurrentPosition()));
}

Token CHTLLexer::scanBlockComment() {
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    advance(2); // 跳过 /*
    
    while (!isAtEnd() && !(current() == '*' && peek(1) == '/')) {
        advance();
    }
    
    if (isAtEnd()) {
        addError("Unterminated block comment", getCurrentPosition());
        return Token(TokenType::UNKNOWN, "", SourceLocation(getCurrentPosition(), getCurrentPosition()));
    }
    
    advance(2); // 跳过 */
    
    StringView value = source_.substr(start, position_ - start);
    return Token(TokenType::BLOCK_COMMENT, value, SourceLocation(startPos, getCurrentPosition()));
}

Token CHTLLexer::scanGeneratorComment() {
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    advance(); // 跳过 #
    
    // 跳过空格
    while (!isAtEnd() && isWhitespace(current())) {
        advance();
    }
    
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
    
    StringView value = source_.substr(start, position_ - start);
    return Token(TokenType::GENERATOR_COMMENT, value, SourceLocation(startPos, getCurrentPosition()));
}

// 扫描操作符
Token CHTLLexer::scanOperator() {
    size_t start = position_;
    Position startPos = getCurrentPosition();
    
    char c = current();
    advance();
    
    // 检查双字符操作符
    if (!isAtEnd()) {
        char next = current();
        String twoChar = String(1, c) + String(1, next);
        
        if (operatorKeywords_.find(twoChar) != operatorKeywords_.end()) {
            advance();
            return Token(operatorKeywords_.at(twoChar), twoChar, SourceLocation(startPos, getCurrentPosition()));
        }
    }
    
    // 单字符操作符
    String singleChar(1, c);
    if (operatorKeywords_.find(singleChar) != operatorKeywords_.end()) {
        return Token(operatorKeywords_.at(singleChar), singleChar, SourceLocation(startPos, getCurrentPosition()));
    }
    
    return Token(TokenType::UNKNOWN, singleChar, SourceLocation(startPos, getCurrentPosition()));
}

// 扫描分隔符
Token CHTLLexer::scanDelimiter() {
    char c = current();
    advance();
    
    TokenType type = TokenType::UNKNOWN;
    switch (c) {
        case '{': type = TokenType::LEFT_BRACE; break;
        case '}': type = TokenType::RIGHT_BRACE; break;
        case '(': type = TokenType::LEFT_PAREN; break;
        case ')': type = TokenType::RIGHT_PAREN; break;
        case '[': type = TokenType::LEFT_BRACKET; break;
        case ']': type = TokenType::RIGHT_BRACKET; break;
        case ';': type = TokenType::SEMICOLON; break;
        case ',': type = TokenType::COMMA; break;
        case '.': type = TokenType::DOT; break;
        case ':': type = TokenType::COLON; break;
        case '=': type = TokenType::EQUALS; break;
    }
    
    return Token(type, String(1, c), SourceLocation(getCurrentPosition(), getCurrentPosition()));
}

// 扫描特殊字符
Token CHTLLexer::scanSpecial() {
    char c = current();
    advance();
    
    TokenType type = TokenType::UNKNOWN;
    switch (c) {
        case '@': type = TokenType::AT_SYMBOL; break;
        case '$': type = TokenType::DOLLAR_SYMBOL; break;
        case '#': type = TokenType::HASH_SYMBOL; break;
        case '?': type = TokenType::QUESTION_MARK; break;
        case '!': type = TokenType::EXCLAMATION; break;
        case '_': type = TokenType::UNDERSCORE; break;
        case '|': type = TokenType::PIPE; break;
        case '~': type = TokenType::TILDE; break;
        case '^': type = TokenType::CARET; break;
    }
    
    return Token(type, String(1, c), SourceLocation(getCurrentPosition(), getCurrentPosition()));
}

// 跳过空白字符
void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && (isWhitespace(current()) || isNewline(current()))) {
        advance();
    }
}

// 错误处理
void CHTLLexer::addError(const String& message, const SourceLocation& location) {
    errors_.emplace_back(ErrorType::LexicalError, message, location);
}

void CHTLLexer::addError(const String& message, const Position& position) {
    errors_.emplace_back(ErrorType::LexicalError, message, SourceLocation(position, position));
}

// 重置方法
void CHTLLexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    tokens_.clear();
    errors_.clear();
}

void CHTLLexer::reset(const StringView& source) {
    source_ = source;
    reset();
}

// 静态方法实现
TokenType CHTLLexer::getKeywordType(const String& keyword) {
    auto it = keywords_.find(keyword);
    if (it != keywords_.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

bool CHTLLexer::isKeyword(const String& word) {
    return keywords_.find(word) != keywords_.end();
}

// 调试方法
void CHTLLexer::printTokens() const {
    std::cout << "Tokens:\n";
    for (const auto& token : tokens_) {
        std::cout << "  " << static_cast<int>(token.type) << ": " << token.value << "\n";
    }
}

void CHTLLexer::printErrors() const {
    if (errors_.empty()) {
        std::cout << "No errors found.\n";
        return;
    }
    
    std::cout << "Errors:\n";
    for (const auto& error : errors_) {
        std::cout << "  Line " << error.location.start.line 
                  << ", Column " << error.location.start.column 
                  << ": " << error.message << "\n";
    }
}

} // namespace CHTL