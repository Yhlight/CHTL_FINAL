#include "CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() : position(0), line(1), column(1) {
    initializeKeywords();
}

CHTLLexer::CHTLLexer(const std::string& source) 
    : source(source), position(0), line(1), column(1) {
    initializeKeywords();
}

void CHTLLexer::initializeKeywords() {
    keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"use", TokenType::USE},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"except", TokenType::EXCEPT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP}, // 需要特殊处理 "at top"
        {"html5", TokenType::HTML5}
    };
}

void CHTLLexer::setSource(const std::string& source) {
    this->source = source;
    position = 0;
    line = 1;
    column = 1;
}

TokenStream CHTLLexer::tokenize() {
    TokenStream stream;
    
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) {
            break;
        }
        
        char c = currentChar();
        
        // 检查CE对等式 (:=)
        if (isColonEqual()) {
            stream.addToken(scanColonEqual());
        }
        // 检查元素名称
        else if (isElementStart()) {
            stream.addToken(scanElementName());
        }
        // 检查属性名称
        else if (isAttributeStart()) {
            stream.addToken(scanAttributeName());
        }
        // 检查文本内容
        else if (isTextStart()) {
            stream.addToken(scanTextContent());
        }
        // 检查text块
        else if (c == 't' && peekChar() == 'e' && peekChar(2) == 'x' && peekChar(3) == 't') {
            // 检查是否为text关键字
            std::string keyword = "text";
            for (size_t i = 0; i < keyword.length(); i++) {
                if (position + i >= source.length() || source[position + i] != keyword[i]) {
                    keyword = "";
                    break;
                }
            }
            if (!keyword.empty()) {
                // 跳过text关键字
                for (size_t i = 0; i < keyword.length(); i++) {
                    advance();
                }
                skipWhitespace();
                
                // 检查是否为text块或text属性
                if (currentChar() == '{') {
                    stream.addToken(Token(TokenType::TEXT, "text", line, column - 4, position - 4));
                } else if (currentChar() == ':') {
                    stream.addToken(Token(TokenType::TEXT, "text", line, column - 4, position - 4));
                } else {
                    // 回退
                    position -= keyword.length();
                    stream.addToken(scanIdentifier());
                }
            } else {
                stream.addToken(scanIdentifier());
            }
        }
        // 检查模板语法 [Template], [Custom], [Origin] 等
        else if (c == '[') {
            stream.addToken(scanTemplateSyntax());
        }
        // 检查模板类型 @Style, @Element, @Var
        else if (c == '@') {
            stream.addToken(scanTemplateType());
        }
        // 检查标识符
        else if (isAlpha(c) || c == '_') {
            stream.addToken(scanIdentifier());
        }
        // 检查字符串
        else if (isQuote(c)) {
            stream.addToken(scanQuotedLiteral());
        }
        // 检查数字
        else if (isDigit(c)) {
            stream.addToken(scanNumber());
        }
        // 检查注释
        else if (c == '/' && peekChar() == '/') {
            stream.addToken(scanComment());
        } else if (c == '/' && peekChar() == '*') {
            stream.addToken(scanComment());
        } else if (c == '-' && peekChar() == '-') {
            stream.addToken(scanComment());
        }
        // 检查局部样式块语法
        else if (c == '.') {
            stream.addToken(scanClassSelector());
        } else if (c == '#') {
            stream.addToken(scanIdSelector());
        } else if (c == '&') {
            stream.addToken(scanContextReference());
        } else if (c == '?' && peekChar() == ':') {
            stream.addToken(scanConditionalExpression());
        }
        // 检查响应式值
        /*else if (c == '$') {
            stream.addToken(scanResponsiveValue());
        }*/
        // 检查运算符
        else if (isOperatorChar(c)) {
            stream.addToken(scanOperator());
        }
        // 检查符号
        else if (isSymbolChar(c)) {
            stream.addToken(scanSymbol());
        }
        // 检查模板关键字
        else if (c == '[') {
            // 处理模板关键字
            size_t start = position;
            advance(); // 跳过 '['
            
            std::string keyword;
            while (position < source.length() && 
                   currentChar() != ']' && 
                   !isWhitespace(currentChar())) {
                keyword += currentChar();
                advance();
            }
            
            if (currentChar() == ']') {
                advance(); // 跳过 ']'
                
                TokenType type = TokenType::UNKNOWN;
                if (keyword == "Template") {
                    type = TokenType::TEMPLATE;
                } else if (keyword == "Custom") {
                    type = TokenType::CUSTOM;
                } else if (keyword == "Origin") {
                    type = TokenType::ORIGIN;
                } else if (keyword == "Import") {
                    type = TokenType::IMPORT;
                } else if (keyword == "Namespace") {
                    type = TokenType::NAMESPACE;
                } else if (keyword == "Configuration") {
                    type = TokenType::CONFIGURATION;
                }
                
                stream.addToken(Token(type, "[" + keyword + "]", line, column - keyword.length() - 1, start));
            } else {
                // 错误：未找到匹配的 ']'
                stream.addToken(Token(TokenType::UNKNOWN, "[" + keyword, line, column - keyword.length() - 1, start));
            }
        }
        // 检查字面量
        else if (isLiteralChar(c)) {
            stream.addToken(scanUnquotedLiteral());
        }
        else {
            // 未知字符
            stream.addToken(Token(TokenType::UNKNOWN, std::string(1, c), line, column, position));
            advance();
        }
    }
    
    // 添加文件结束标记
    stream.addToken(Token(TokenType::END_OF_FILE, "", line, column, position));
    
    return stream;
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
}

char CHTLLexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLLexer::peekChar(size_t offset) const {
    if (position + offset >= source.length()) {
        return '\0';
    }
    return source[position + offset];
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
    while (position < source.length() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLLexer::skipLine() {
    while (position < source.length() && currentChar() != '\n') {
        advance();
    }
    if (position < source.length()) {
        advance(); // 跳过换行符
    }
}

Token CHTLLexer::scanIdentifier() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    while (position < source.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    
    // 检查是否为关键字
    auto it = keywords.find(value);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    
    // 特殊处理 "at top" 和 "at bottom"
    if (value == "at") {
        skipWhitespace();
        if (position < source.length()) {
            std::string nextWord;
            size_t nextStart = position;
            while (position < source.length() && 
                   (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
                nextWord += currentChar();
                advance();
            }
            
            if (nextWord == "top") {
                type = TokenType::AT_TOP;
                value = "at top";
            } else if (nextWord == "bottom") {
                type = TokenType::AT_BOTTOM;
                value = "at bottom";
            } else {
                // 回退到 "at" 的位置
                position = nextStart;
                advance();
            }
        }
    }
    
    return Token(type, value, startLine, startColumn, start);
}

Token CHTLLexer::scanString() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (position < source.length() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar() == quote) {
            advance(); // 跳过反斜杠
            value += currentChar();
            advance();
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (position < source.length()) {
        advance(); // 跳过结束引号
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn, start);
}

Token CHTLLexer::scanNumber() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    // 整数部分
    while (position < source.length() && isDigit(currentChar())) {
        value += currentChar();
        advance();
    }
    
    // 小数部分
    if (position < source.length() && currentChar() == '.') {
        value += currentChar();
        advance();
        
        while (position < source.length() && isDigit(currentChar())) {
            value += currentChar();
            advance();
        }
    }
    
    // 科学计数法
    if (position < source.length() && (currentChar() == 'e' || currentChar() == 'E')) {
        value += currentChar();
        advance();
        
        if (position < source.length() && (currentChar() == '+' || currentChar() == '-')) {
            value += currentChar();
            advance();
        }
        
        while (position < source.length() && isDigit(currentChar())) {
            value += currentChar();
            advance();
        }
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, start);
}

Token CHTLLexer::scanLiteral() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (position < source.length() && 
           !isWhitespace(currentChar()) && 
           !isSymbolChar(currentChar()) &&
           currentChar() != ';' &&
           currentChar() != ':' &&
           currentChar() != '=') {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::LITERAL, value, startLine, startColumn, start);
}

Token CHTLLexer::scanComment() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        value += currentChar();
        advance();
        value += currentChar();
        advance();
        
        while (position < source.length() && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::SINGLE_COMMENT, value, startLine, startColumn, start);
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        value += currentChar();
        advance();
        value += currentChar();
        advance();
        
        while (position < source.length()) {
            if (currentChar() == '*' && peekChar() == '/') {
                value += currentChar();
                advance();
                value += currentChar();
                advance();
                break;
            }
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::MULTI_COMMENT, value, startLine, startColumn, start);
    } else if (currentChar() == '-' && peekChar() == '-') {
        // 生成器注释
        value += currentChar();
        advance();
        value += currentChar();
        advance();
        
        while (position < source.length() && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, start);
    }
    
    return Token(TokenType::UNKNOWN, value, startLine, startColumn, start);
}

Token CHTLLexer::scanOperator() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = currentChar();
    advance();
    
    if (position < source.length()) {
        char next = currentChar();
        
        // 双字符运算符
        if ((c == '&' && next == '&') ||
            (c == '|' && next == '|') ||
            (c == '=' && next == '=') ||
            (c == '!' && next == '=') ||
            (c == '<' && next == '=') ||
            (c == '>' && next == '=') ||
            (c == '+' && next == '+') ||
            (c == '-' && next == '-') ||
            (c == '*' && next == '*') ||
            (c == '-' && next == '>') ||
            (c == '=' && next == '>')) {
            
            advance();
            std::string value = std::string(1, c) + next;
            
            TokenType type = TokenType::UNKNOWN;
            if (c == '&' && next == '&') type = TokenType::AND;
            else if (c == '|' && next == '|') type = TokenType::OR;
            else if (c == '=' && next == '=') type = TokenType::EQUAL_EQUAL;
            else if (c == '!' && next == '=') type = TokenType::NOT_EQUAL;
            else if (c == '<' && next == '=') type = TokenType::LESS_EQUAL;
            else if (c == '>' && next == '=') type = TokenType::GREATER_EQUAL;
            else if (c == '+' && next == '+') type = TokenType::INCREMENT;
            else if (c == '-' && next == '-') type = TokenType::DECREMENT;
            else if (c == '*' && next == '*') type = TokenType::POWER;
            else if (c == '-' && next == '>') type = TokenType::ARROW;
            else if (c == '=' && next == '>') type = TokenType::DOUBLE_ARROW;
            
            return Token(type, value, startLine, startColumn, start);
        }
    }
    
    // 单字符运算符
    TokenType type = TokenType::UNKNOWN;
    switch (c) {
        case '+': type = TokenType::PLUS; break;
        case '-': type = TokenType::MINUS; break;
        case '*': type = TokenType::ASTERISK; break;
        case '/': type = TokenType::SLASH; break;
        case '%': type = TokenType::PERCENT; break;
        case '^': type = TokenType::CARET; break;
        case '=': type = TokenType::EQUAL; break;
        case '<': type = TokenType::LESS; break;
        case '>': type = TokenType::GREATER; break;
        case '&': type = TokenType::AMPERSAND; break;
        case '|': type = TokenType::PIPE; break;
    }
    
    return Token(type, std::string(1, c), startLine, startColumn, start);
}

Token CHTLLexer::scanSymbol() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = currentChar();
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
        case ':': type = TokenType::COLON; break;
        case ',': type = TokenType::COMMA; break;
        case '.': type = TokenType::DOT; break;
        case '#': type = TokenType::HASH; break;
        case '?': type = TokenType::QUESTION; break;
        case '!': type = TokenType::EXCLAMATION; break;
        case '\\': type = TokenType::BACKSLASH; break;
        case '~': type = TokenType::TILDE; break;
        case '@': type = TokenType::AT; break;
        case '$': type = TokenType::DOLLAR; break;
        case '_': type = TokenType::UNDERSCORE; break;
    }
    
    return Token(type, std::string(1, c), startLine, startColumn, start);
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n';
}

bool CHTLLexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isOperatorChar(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || 
           c == '^' || c == '=' || c == '<' || c == '>' || c == '&' || c == '|';
}

bool CHTLLexer::isSymbolChar(char c) const {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
           c == ';' || c == ':' || c == ',' || c == '.' || c == '#' || c == '?' ||
           c == '!' || c == '\\' || c == '~' || c == '@' || c == '$' || c == '_';
}

bool CHTLLexer::isTemplateKeyword(const std::string& str) const {
    return str == "Template";
}

bool CHTLLexer::isCustomKeyword(const std::string& str) const {
    return str == "Custom";
}

bool CHTLLexer::isOriginKeyword(const std::string& str) const {
    return str == "Origin";
}

bool CHTLLexer::isImportKeyword(const std::string& str) const {
    return str == "Import";
}

bool CHTLLexer::isNamespaceKeyword(const std::string& str) const {
    return str == "Namespace";
}

bool CHTLLexer::isConfigurationKeyword(const std::string& str) const {
    return str == "Configuration";
}

bool CHTLLexer::isUseKeyword(const std::string& str) const {
    return str == "use";
}

bool CHTLLexer::isHTML5Keyword(const std::string& str) const {
    return str == "html5";
}

// CHTL特有词法分析方法实现
Token CHTLLexer::scanElementName() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (position < source.length() && isElementNameChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::ELEMENT_NAME, value, startLine, startColumn, start);
}

Token CHTLLexer::scanAttributeName() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (position < source.length() && isAttributeNameChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::ATTRIBUTE_NAME, value, startLine, startColumn, start);
}

Token CHTLLexer::scanAttributeValue() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    // 跳过等号
    if (currentChar() == '=') {
        advance();
        skipWhitespace();
    }
    
    // 检查是否有引号
    if (isQuote(currentChar())) {
        return scanQuotedLiteral();
    } else {
        // 无引号属性值
        while (position < source.length() && 
               !isWhitespace(currentChar()) && 
               currentChar() != ';' && 
               currentChar() != ':' &&
               currentChar() != '=') {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::ATTRIBUTE_VALUE, value, startLine, startColumn, start);
    }
}

Token CHTLLexer::scanTextContent() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (position < source.length() && isTextContentChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::TEXT_CONTENT, value, startLine, startColumn, start);
}

Token CHTLLexer::scanUnquotedLiteral() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (position < source.length() && isLiteralChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn, start);
}

Token CHTLLexer::scanQuotedLiteral() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (position < source.length() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar() == quote) {
            advance(); // 跳过反斜杠
            value += currentChar();
            advance();
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (position < source.length()) {
        advance(); // 跳过结束引号
    }
    
    TokenType type = (quote == '\'') ? TokenType::SINGLE_QUOTED_LITERAL : TokenType::DOUBLE_QUOTED_LITERAL;
    return Token(type, value, startLine, startColumn, start);
}

Token CHTLLexer::scanColonEqual() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 ':'
    advance(); // 跳过 '='
    
    return Token(TokenType::COLON_EQUAL, ":=", startLine, startColumn, start);
}

// CHTL特有检查方法实现
bool CHTLLexer::isElementNameChar(char c) const {
    return isAlpha(c) || isDigit(c) || c == '-' || c == '_';
}

bool CHTLLexer::isAttributeNameChar(char c) const {
    return isAlpha(c) || isDigit(c) || c == '-' || c == '_' || c == '.';
}

bool CHTLLexer::isTextContentChar(char c) const {
    return !isWhitespace(c) && c != ';' && c != ':' && c != '=' && c != '{' && c != '}';
}

bool CHTLLexer::isLiteralChar(char c) const {
    return !isWhitespace(c) && !isSymbolChar(c) && !isOperatorChar(c) && c != ';' && c != ':' && c != '=';
}

bool CHTLLexer::isColonEqual() const {
    return currentChar() == ':' && peekChar() == '=';
}

bool CHTLLexer::isElementStart() const {
    char c = currentChar();
    return isAlpha(c) || c == '_';
}

bool CHTLLexer::isAttributeStart() const {
    char c = currentChar();
    return isAlpha(c) || c == '_' || c == '.';
}

bool CHTLLexer::isTextStart() const {
    char c = currentChar();
    // 文本内容不能以特殊字符开始，但可以是字母、数字、中文等
    return !isWhitespace(c) && !isSymbolChar(c) && !isOperatorChar(c) && 
           c != ';' && c != ':' && c != '=' && c != '[' && c != '/' && c != '-' &&
           c != '\0' && c != '\n' && c != '\r' && c != '\t';
}

// 局部样式块词法分析方法
Token CHTLLexer::scanClassSelector() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '.'
    
    std::string className;
    while (position < source.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '-' || currentChar() == '_')) {
        className += currentChar();
        advance();
    }
    
    return Token(TokenType::CLASS_SELECTOR, className, startLine, startColumn, start);
}

Token CHTLLexer::scanIdSelector() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '#'
    
    std::string idName;
    while (position < source.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '-' || currentChar() == '_')) {
        idName += currentChar();
        advance();
    }
    
    return Token(TokenType::ID_SELECTOR, idName, startLine, startColumn, start);
}

Token CHTLLexer::scanContextReference() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '&'
    
    return Token(TokenType::CONTEXT_REF, "&", startLine, startColumn, start);
}

Token CHTLLexer::scanConditionalExpression() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '?'
    advance(); // 跳过 ':'
    
    return Token(TokenType::CONDITIONAL_EXPR, "?:", startLine, startColumn, start);
}

Token CHTLLexer::scanPropertyReference() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string reference;
    
    // 解析选择器部分
    while (position < source.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '.' || 
            currentChar() == '#' || currentChar() == ' ' || currentChar() == '[' || 
            currentChar() == ']' || currentChar() == '-' || currentChar() == '_')) {
        reference += currentChar();
        advance();
    }
    
    // 检查是否有属性部分
    if (position < source.length() && currentChar() == '.') {
        reference += currentChar();
        advance();
        
        // 解析属性名
        while (position < source.length() && 
               (isAlphaNumeric(currentChar()) || currentChar() == '-' || currentChar() == '_')) {
            reference += currentChar();
            advance();
        }
    }
    
    return Token(TokenType::PROPERTY_REF, reference, startLine, startColumn, start);
}

Token CHTLLexer::scanTemplateSyntax() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '['
    
    std::string syntax;
    while (position < source.length() && currentChar() != ']') {
        syntax += currentChar();
        advance();
    }
    
    if (position < source.length() && currentChar() == ']') {
        advance(); // 跳过 ']'
    }
    
    // 确定Token类型
    TokenType type = TokenType::IDENTIFIER;
    if (syntax == "Template") {
        type = TokenType::TEMPLATE;
    } else if (syntax == "Custom") {
        type = TokenType::CUSTOM;
    } else if (syntax == "Origin") {
        type = TokenType::ORIGIN;
    } else if (syntax == "Import") {
        type = TokenType::IMPORT;
    } else if (syntax == "Namespace") {
        type = TokenType::NAMESPACE;
    } else if (syntax == "Configuration") {
        type = TokenType::CONFIGURATION;
    } else if (syntax == "Constraint") {
        type = TokenType::CONSTRAINT;
    } else if (syntax == "Use") {
        type = TokenType::USE;
    }
    
    return Token(type, syntax, startLine, startColumn, start);
}

Token CHTLLexer::scanTemplateType() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '@'
    
    std::string type;
    while (position < source.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        type += currentChar();
        advance();
    }
    
    // 确定Token类型
    TokenType tokenType = TokenType::IDENTIFIER;
    if (type == "Style") {
        tokenType = TokenType::TEMPLATE_STYLE;
    } else if (type == "Element") {
        tokenType = TokenType::TEMPLATE_ELEMENT;
    } else if (type == "Var") {
        tokenType = TokenType::TEMPLATE_VAR;
    }
    
    return Token(tokenType, type, startLine, startColumn, start);
}

Token CHTLLexer::scanResponsiveValue() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    advance(); // 跳过 '$'
    
    std::string value;
    while (position < source.length() && 
           (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }
    
    if (currentChar() == '$') {
        advance(); // 跳过结束的 '$'
        return Token(TokenType::RESPONSIVE_VALUE, value, startLine, startColumn, start);
    } else {
        // 错误：未找到匹配的 '$'
        return Token(TokenType::UNKNOWN, "$" + value, startLine, startColumn, start);
    }
}

} // namespace CHTL