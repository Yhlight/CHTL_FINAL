#include "Lexer.hpp"
#include <cctype>
#include <stdexcept>

namespace CHTL {

Lexer::Lexer(const std::string& source) 
    : source_(source), current_(0), line_(1), column_(1), 
      start_(0), startLine_(1), startColumn_(1) {}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (current_ >= source_.length()) {
        return createToken(TokenType::END_OF_FILE, "");
    }
    
    start_ = current_;
    startLine_ = line_;
    startColumn_ = column_;
    
    char c = currentChar();
    
    // 处理注释
    if (c == '/' && current_ + 1 < source_.length()) {
        char next = source_[current_ + 1];
        if (next == '/') {
            skipSingleLineComment();
            return createToken(TokenType::SINGLE_COMMENT, "//");
        } else if (next == '*') {
            skipMultiLineComment();
            return createToken(TokenType::MULTI_COMMENT, "/* */");
        }
    }
    
    // 处理生成器注释
    if (c == '-' && current_ + 1 < source_.length() && source_[current_ + 1] == '-') {
        skipGeneratorComment();
        return createToken(TokenType::GENERATOR_COMMENT, "--");
    }
    
    // 处理字符串字面量
    if (isQuote(c)) {
        return readStringLiteral();
    }
    
    // 处理数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 处理标识符或关键字
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 处理方括号关键字
    if (c == '[') {
        return readBracketKeyword();
    }
    
    // 处理双大括号增强选择器
    if (c == '{' && current_ + 1 < source_.length() && source_[current_ + 1] == '{') {
        return readEnhancedSelector();
    }
    
    // 处理操作符
    return readOperator();
}

Token Lexer::peekToken() {
    size_t savedCurrent = current_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    
    Token token = nextToken();
    
    current_ = savedCurrent;
    line_ = savedLine;
    column_ = savedColumn;
    
    return token;
}

Token Lexer::peekNextToken() {
    size_t savedCurrent = current_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    
    nextToken(); // 跳过当前token
    Token token = nextToken();
    
    current_ = savedCurrent;
    line_ = savedLine;
    column_ = savedColumn;
    
    return token;
}

void Lexer::reset() {
    current_ = 0;
    line_ = 1;
    column_ = 1;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    reset();
    
    Token token = nextToken();
    while (token.getType() != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = nextToken();
    }
    tokens.push_back(token); // 添加 EOF token
    
    return tokens;
}

void Lexer::skipWhitespace() {
    while (current_ < source_.length() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        current_++;
    }
}

void Lexer::skipComment() {
    if (current_ + 1 < source_.length()) {
        char next = source_[current_ + 1];
        if (next == '/') {
            skipSingleLineComment();
        } else if (next == '*') {
            skipMultiLineComment();
        }
    }
}

void Lexer::skipSingleLineComment() {
    while (current_ < source_.length() && currentChar() != '\n') {
        current_++;
        column_++;
    }
}

void Lexer::skipMultiLineComment() {
    current_ += 2; // 跳过 /*
    column_ += 2;
    
    while (current_ < source_.length()) {
        if (currentChar() == '*' && current_ + 1 < source_.length() && 
            source_[current_ + 1] == '/') {
            current_ += 2; // 跳过 */
            column_ += 2;
            break;
        }
        
        if (currentChar() == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        current_++;
    }
}

void Lexer::skipGeneratorComment() {
    current_ += 2; // 跳过 --
    column_ += 2;
    
    while (current_ < source_.length() && currentChar() != '\n') {
        current_++;
        column_++;
    }
}

Token Lexer::readStringLiteral() {
    char quote = currentChar();
    current_++;
    column_++;
    
    std::string value;
    while (current_ < source_.length() && currentChar() != quote) {
        if (currentChar() == '\\' && current_ + 1 < source_.length()) {
            current_++;
            column_++;
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
        } else {
            value += currentChar();
        }
        current_++;
        column_++;
    }
    
    if (current_ < source_.length()) {
        current_++; // 跳过结束引号
        column_++;
    }
    
    return createToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::readUnquotedLiteral() {
    std::string value;
    
    // 检查是否为属性条件表达式
    bool isPropertyExpression = false;
    bool hasArithmetic = false;
    bool hasConditional = false;
    bool hasPropertyReference = false;
    
    while (current_ < source_.length()) {
        char c = currentChar();
        
        // 检查算术运算符
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
            hasArithmetic = true;
            isPropertyExpression = true;
        }
        
        // 检查条件运算符
        if (c == '?' || c == ':') {
            hasConditional = true;
            isPropertyExpression = true;
        }
        
        // 检查逻辑运算符
        if (c == '&' && current_ + 1 < source_.length() && source_[current_ + 1] == '&') {
            hasConditional = true;
            isPropertyExpression = true;
        }
        if (c == '|' && current_ + 1 < source_.length() && source_[current_ + 1] == '|') {
            hasConditional = true;
            isPropertyExpression = true;
        }
        
        // 检查比较运算符
        if (c == '<' || c == '>' || c == '=' || c == '!') {
            hasConditional = true;
            isPropertyExpression = true;
        }
        
        // 检查属性引用（选择器.属性）
        if (c == '.' && !value.empty() && 
            (isAlpha(value.back()) || isDigit(value.back()) || value.back() == '_')) {
            hasPropertyReference = true;
            isPropertyExpression = true;
        }
        
        // 检查动态引用（{{var}}->property）
        if (c == '-' && current_ + 1 < source_.length() && source_[current_ + 1] == '>') {
            isPropertyExpression = true;
        }
        
        // 如果遇到结束符号且不是属性表达式，则停止
        if (!isPropertyExpression && 
            (isWhitespace(c) || c == ';' || c == ':' || c == '=' ||
             c == '{' || c == '}' || c == '[' || c == ']' ||
             c == '(' || c == ')' || c == ',' || c == '#' ||
             c == '@' || c == '&' || c == '!' || c == '_' ||
             c == '/' || c == '\\' || c == '|' || c == '~' ||
             c == '^' || c == '$' || c == '%')) {
            break;
        }
        
        // 如果是属性表达式，继续读取直到遇到分号或右大括号
        if (isPropertyExpression && (c == ';' || c == '}')) {
            break;
        }
        
        value += c;
        current_++;
        column_++;
    }
    
    // 根据内容确定Token类型
    if (isPropertyExpression) {
        if (hasPropertyReference || hasArithmetic || hasConditional) {
            return createToken(TokenType::PROPERTY_CONDITIONAL, value);
        }
    }
    
    return createToken(TokenType::UNQUOTED_LITERAL, value);
}

Token Lexer::readNumber() {
    std::string value;
    
    while (current_ < source_.length() && 
           (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        current_++;
        column_++;
    }
    
    return createToken(TokenType::NUMBER, value);
}

Token Lexer::readIdentifier() {
    std::string value;
    
    while (current_ < source_.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        current_++;
        column_++;
    }
    
    // 检查是否为关键字
    TokenType type = KeywordMap::getKeywordType(value);
    if (type != TokenType::IDENTIFIER) {
        return createToken(type, value);
    }
    
    return createToken(TokenType::IDENTIFIER, value);
}

Token Lexer::readOperator() {
    char c = currentChar();
    
    // 处理双大括号 {{ }}
    if (c == '{' && current_ + 1 < source_.length() && source_[current_ + 1] == '{') {
        current_ += 2;
        column_ += 2;
        return createToken(TokenType::DOUBLE_BRACE_LEFT, "{{");
    }
    
    if (c == '}' && current_ + 1 < source_.length() && source_[current_ + 1] == '}') {
        current_ += 2;
        column_ += 2;
        return createToken(TokenType::DOUBLE_BRACE_RIGHT, "}}");
    }
    
    // 处理多字符操作符
    if (current_ + 1 < source_.length()) {
        std::string twoChar = std::string(1, c) + source_[current_ + 1];
        if (OperatorMap::isOperator(twoChar)) {
            current_ += 2;
            column_ += 2;
            return createToken(OperatorMap::getOperatorType(twoChar), twoChar);
        }
    }
    
    if (current_ + 2 < source_.length()) {
        std::string threeChar = std::string(1, c) + source_[current_ + 1] + source_[current_ + 2];
        if (OperatorMap::isOperator(threeChar)) {
            current_ += 3;
            column_ += 3;
            return createToken(OperatorMap::getOperatorType(threeChar), threeChar);
        }
    }
    
    // 处理单字符操作符
    if (OperatorMap::isOperator(std::string(1, c))) {
        current_++;
        column_++;
        return createToken(OperatorMap::getOperatorType(std::string(1, c)), std::string(1, c));
    }
    
    // 处理无修饰字面量
    if (!isWhitespace(c) && !isQuote(c)) {
        return readUnquotedLiteral();
    }
    
    return createErrorToken("Unexpected character: " + std::string(1, c));
}

Token Lexer::readTemplateKeyword() {
    std::string value;
    
    while (current_ < source_.length() && currentChar() != ']') {
        value += currentChar();
        current_++;
        column_++;
    }
    
    if (current_ < source_.length()) {
        value += currentChar(); // 添加 ]
        current_++;
        column_++;
    }
    
    return createToken(KeywordMap::getKeywordType(value), value);
}

Token Lexer::readBracketKeyword() {
    std::string value;
    value += currentChar(); // 添加 [
    current_++;
    column_++;
    
    while (current_ < source_.length() && currentChar() != ']') {
        value += currentChar();
        current_++;
        column_++;
    }
    
    if (current_ < source_.length()) {
        value += currentChar(); // 添加 ]
        current_++;
        column_++;
    }
    
    return createToken(KeywordMap::getKeywordType(value), value);
}

Token Lexer::readEnhancedSelector() {
    std::string value;
    
    // 添加 {{
    value += currentChar();
    current_++;
    column_++;
    value += currentChar();
    current_++;
    column_++;
    
    // 读取选择器内容
    while (current_ < source_.length()) {
        char c = currentChar();
        if (c == '}' && current_ + 1 < source_.length() && source_[current_ + 1] == '}') {
            // 找到结束标记 }}
            value += c;
            current_++;
            column_++;
            value += currentChar();
            current_++;
            column_++;
            break;
        } else {
            value += c;
            current_++;
            column_++;
        }
    }
    
    return createToken(TokenType::ENHANCED_SELECTOR, value);
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
    return std::isspace(c);
}

bool Lexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

char Lexer::currentChar() const {
    return (current_ < source_.length()) ? source_[current_] : '\0';
}

char Lexer::nextChar() {
    if (current_ < source_.length()) {
        current_++;
        if (source_[current_ - 1] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        return currentChar();
    }
    return '\0';
}

void Lexer::ungetChar() {
    if (current_ > 0) {
        current_--;
        if (source_[current_] == '\n') {
            line_--;
            // 精确计算列号：从当前位置向前查找直到遇到换行符
            column_ = 1;
            for (int i = current_ - 1; i >= 0; i--) {
                if (source_[i] == '\n') {
                    column_ = current_ - i;
                    break;
                }
            }
        } else {
            column_--;
        }
    }
}

Token Lexer::createToken(TokenType type, const std::string& value) {
    return Token(type, value, startLine_, startColumn_);
}

Token Lexer::createErrorToken(const std::string& message) {
    return Token(TokenType::ERROR, message, startLine_, startColumn_);
}

} // namespace CHTL