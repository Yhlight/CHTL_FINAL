#include "CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source) 
    : source(source), position(0), line(1), column(1), start(0) {
    initializeKeywords();
    initializeOperators();
    initializeHTMLElements();
}

void CHTLLexer::initializeKeywords() {
    keywords = {
        // CHTL 关键字
        {"inherit", CHTLTokenType::KEYWORD_INHERIT},
        {"delete", CHTLTokenType::KEYWORD_DELETE},
        {"insert", CHTLTokenType::KEYWORD_INSERT},
        {"after", CHTLTokenType::KEYWORD_AFTER},
        {"before", CHTLTokenType::KEYWORD_BEFORE},
        {"replace", CHTLTokenType::KEYWORD_REPLACE},
        {"at", CHTLTokenType::KEYWORD_ATTOP},
        {"top", CHTLTokenType::KEYWORD_ATTOP},
        {"bottom", CHTLTokenType::KEYWORD_ATBOTTOM},
        {"from", CHTLTokenType::KEYWORD_FROM},
        {"as", CHTLTokenType::KEYWORD_AS},
        {"except", CHTLTokenType::KEYWORD_EXCEPT},
        {"use", CHTLTokenType::KEYWORD_USE},
        {"html5", CHTLTokenType::KEYWORD_HTML5},
        {"text", CHTLTokenType::KEYWORD_TEXT},
        {"style", CHTLTokenType::KEYWORD_STYLE},
        {"script", CHTLTokenType::KEYWORD_SCRIPT},
        
        // 模板类型
        {"@Style", CHTLTokenType::TEMPLATE_STYLE},
        {"@Element", CHTLTokenType::TEMPLATE_ELEMENT},
        {"@Var", CHTLTokenType::TEMPLATE_VAR},
        
        // 原始嵌入类型
        {"@Html", CHTLTokenType::ORIGIN_HTML},
        {"@Style", CHTLTokenType::ORIGIN_STYLE},
        {"@JavaScript", CHTLTokenType::ORIGIN_JAVASCRIPT},
        
        // 导入类型
        {"@Html", CHTLTokenType::IMPORT_HTML},
        {"@Style", CHTLTokenType::IMPORT_STYLE},
        {"@JavaScript", CHTLTokenType::IMPORT_JAVASCRIPT},
        {"@Chtl", CHTLTokenType::IMPORT_CHTL},
        {"@CJmod", CHTLTokenType::IMPORT_CJMOD},
        
        // 布尔值
        {"true", CHTLTokenType::BOOLEAN},
        {"false", CHTLTokenType::BOOLEAN},
        {"null", CHTLTokenType::BOOLEAN},
        {"undefined", CHTLTokenType::BOOLEAN}
    };
}

void CHTLLexer::initializeOperators() {
    operators = {
        {"=", CHTLTokenType::ASSIGN},
        {":", CHTLTokenType::COLON},
        {";", CHTLTokenType::SEMICOLON},
        {",", CHTLTokenType::COMMA},
        {".", CHTLTokenType::DOT},
        {"?", CHTLTokenType::QUESTION},
        {"!", CHTLTokenType::EXCLAMATION},
        {"==", CHTLTokenType::EQUAL},
        {"!=", CHTLTokenType::NOT_EQUAL},
        {"<", CHTLTokenType::LESS},
        {">", CHTLTokenType::GREATER},
        {"<=", CHTLTokenType::LESS_EQUAL},
        {">=", CHTLTokenType::GREATER_EQUAL},
        {"+", CHTLTokenType::PLUS},
        {"-", CHTLTokenType::MINUS},
        {"*", CHTLTokenType::MULTIPLY},
        {"/", CHTLTokenType::DIVIDE},
        {"%", CHTLTokenType::MODULO},
        {"**", CHTLTokenType::POWER},
        {"&&", CHTLTokenType::AND},
        {"||", CHTLTokenType::OR},
        {"(", CHTLTokenType::LEFT_PAREN},
        {")", CHTLTokenType::RIGHT_PAREN},
        {"{", CHTLTokenType::LEFT_BRACE},
        {"}", CHTLTokenType::RIGHT_BRACE},
        {"[", CHTLTokenType::LEFT_BRACKET},
        {"]", CHTLTokenType::RIGHT_BRACKET},
        {"@", CHTLTokenType::AT},
        {"#", CHTLTokenType::HASH},
        {"$", CHTLTokenType::DOLLAR},
        {"_", CHTLTokenType::UNDERSCORE},
        {"~", CHTLTokenType::TILDE},
        {"|", CHTLTokenType::PIPE},
        {"&", CHTLTokenType::AMPERSAND}
    };
}

void CHTLLexer::initializeHTMLElements() {
    htmlElements = {
        // 文档结构
        {"html", CHTLTokenType::HTML_ELEMENT},
        {"head", CHTLTokenType::HTML_ELEMENT},
        {"body", CHTLTokenType::HTML_ELEMENT},
        {"title", CHTLTokenType::HTML_ELEMENT},
        {"meta", CHTLTokenType::HTML_ELEMENT},
        {"link", CHTLTokenType::HTML_ELEMENT},
        {"style", CHTLTokenType::HTML_ELEMENT},
        {"script", CHTLTokenType::HTML_ELEMENT},
        
        // 文本内容
        {"h1", CHTLTokenType::HTML_ELEMENT},
        {"h2", CHTLTokenType::HTML_ELEMENT},
        {"h3", CHTLTokenType::HTML_ELEMENT},
        {"h4", CHTLTokenType::HTML_ELEMENT},
        {"h5", CHTLTokenType::HTML_ELEMENT},
        {"h6", CHTLTokenType::HTML_ELEMENT},
        {"p", CHTLTokenType::HTML_ELEMENT},
        {"span", CHTLTokenType::HTML_ELEMENT},
        {"div", CHTLTokenType::HTML_ELEMENT},
        {"a", CHTLTokenType::HTML_ELEMENT},
        {"strong", CHTLTokenType::HTML_ELEMENT},
        {"em", CHTLTokenType::HTML_ELEMENT},
        {"b", CHTLTokenType::HTML_ELEMENT},
        {"i", CHTLTokenType::HTML_ELEMENT},
        {"u", CHTLTokenType::HTML_ELEMENT},
        {"s", CHTLTokenType::HTML_ELEMENT},
        {"small", CHTLTokenType::HTML_ELEMENT},
        {"mark", CHTLTokenType::HTML_ELEMENT},
        {"del", CHTLTokenType::HTML_ELEMENT},
        {"ins", CHTLTokenType::HTML_ELEMENT},
        {"sub", CHTLTokenType::HTML_ELEMENT},
        {"sup", CHTLTokenType::HTML_ELEMENT},
        
        // 列表
        {"ul", CHTLTokenType::HTML_ELEMENT},
        {"ol", CHTLTokenType::HTML_ELEMENT},
        {"li", CHTLTokenType::HTML_ELEMENT},
        {"dl", CHTLTokenType::HTML_ELEMENT},
        {"dt", CHTLTokenType::HTML_ELEMENT},
        {"dd", CHTLTokenType::HTML_ELEMENT},
        
        // 表格
        {"table", CHTLTokenType::HTML_ELEMENT},
        {"thead", CHTLTokenType::HTML_ELEMENT},
        {"tbody", CHTLTokenType::HTML_ELEMENT},
        {"tfoot", CHTLTokenType::HTML_ELEMENT},
        {"tr", CHTLTokenType::HTML_ELEMENT},
        {"th", CHTLTokenType::HTML_ELEMENT},
        {"td", CHTLTokenType::HTML_ELEMENT},
        {"caption", CHTLTokenType::HTML_ELEMENT},
        {"colgroup", CHTLTokenType::HTML_ELEMENT},
        {"col", CHTLTokenType::HTML_ELEMENT},
        
        // 表单
        {"form", CHTLTokenType::HTML_ELEMENT},
        {"input", CHTLTokenType::HTML_ELEMENT},
        {"textarea", CHTLTokenType::HTML_ELEMENT},
        {"button", CHTLTokenType::HTML_ELEMENT},
        {"select", CHTLTokenType::HTML_ELEMENT},
        {"option", CHTLTokenType::HTML_ELEMENT},
        {"optgroup", CHTLTokenType::HTML_ELEMENT},
        {"label", CHTLTokenType::HTML_ELEMENT},
        {"fieldset", CHTLTokenType::HTML_ELEMENT},
        {"legend", CHTLTokenType::HTML_ELEMENT},
        
        // 媒体
        {"img", CHTLTokenType::HTML_ELEMENT},
        {"video", CHTLTokenType::HTML_ELEMENT},
        {"audio", CHTLTokenType::HTML_ELEMENT},
        {"source", CHTLTokenType::HTML_ELEMENT},
        {"track", CHTLTokenType::HTML_ELEMENT},
        {"canvas", CHTLTokenType::HTML_ELEMENT},
        {"svg", CHTLTokenType::HTML_ELEMENT},
        
        // 语义化
        {"header", CHTLTokenType::HTML_ELEMENT},
        {"nav", CHTLTokenType::HTML_ELEMENT},
        {"main", CHTLTokenType::HTML_ELEMENT},
        {"section", CHTLTokenType::HTML_ELEMENT},
        {"article", CHTLTokenType::HTML_ELEMENT},
        {"aside", CHTLTokenType::HTML_ELEMENT},
        {"footer", CHTLTokenType::HTML_ELEMENT},
        {"address", CHTLTokenType::HTML_ELEMENT},
        
        // 其他
        {"br", CHTLTokenType::HTML_ELEMENT},
        {"hr", CHTLTokenType::HTML_ELEMENT},
        {"pre", CHTLTokenType::HTML_ELEMENT},
        {"code", CHTLTokenType::HTML_ELEMENT},
        {"blockquote", CHTLTokenType::HTML_ELEMENT},
        {"cite", CHTLTokenType::HTML_ELEMENT},
        {"q", CHTLTokenType::HTML_ELEMENT},
        {"abbr", CHTLTokenType::HTML_ELEMENT},
        {"time", CHTLTokenType::HTML_ELEMENT},
        {"data", CHTLTokenType::HTML_ELEMENT},
        {"meter", CHTLTokenType::HTML_ELEMENT},
        {"progress", CHTLTokenType::HTML_ELEMENT},
        {"details", CHTLTokenType::HTML_ELEMENT},
        {"summary", CHTLTokenType::HTML_ELEMENT},
        {"dialog", CHTLTokenType::HTML_ELEMENT},
        {"menu", CHTLTokenType::HTML_ELEMENT},
        {"menuitem", CHTLTokenType::HTML_ELEMENT}
    };
}

char CHTLLexer::current() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLLexer::peek(size_t offset) const {
    size_t index = position + offset;
    if (index >= source.length()) {
        return '\0';
    }
    return source[index];
}

void CHTLLexer::advance() {
    if (position < source.length()) {
        if (current() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(current())) {
        advance();
    }
}

void CHTLLexer::skipComment() {
    if (current() == '/' && peek() == '/') {
        skipLineComment();
    } else if (current() == '/' && peek() == '*') {
        skipBlockComment();
    }
}

void CHTLLexer::skipLineComment() {
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
}

void CHTLLexer::skipBlockComment() {
    advance(); // 跳过 '/'
    advance(); // 跳过 '*'
    
    while (!isAtEnd()) {
        if (current() == '*' && peek() == '/') {
            advance(); // 跳过 '*'
            advance(); // 跳过 '/'
            break;
        }
        advance();
    }
}

void CHTLLexer::skipGeneratorComment() {
    advance(); // 跳过 '#'
    
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
}

CHTLToken CHTLLexer::scanString() {
    char quote = current();
    advance(); // 跳过开始引号
    
    std::string value;
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\') {
            advance(); // 跳过反斜杠
            if (!isAtEnd()) {
                switch (current()) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += current(); break;
                }
                advance();
            }
        } else {
            value += current();
            advance();
        }
    }
    
    if (isAtEnd()) {
        error("Unterminated string");
        return makeToken(CHTLTokenType::UNKNOWN);
    }
    
    advance(); // 跳过结束引号
    return makeToken(CHTLTokenType::STRING, value);
}

CHTLToken CHTLLexer::scanNumber() {
    std::string value;
    
    // 处理整数部分
    while (isDigit(current())) {
        value += current();
        advance();
    }
    
    // 处理小数部分
    if (current() == '.' && isDigit(peek())) {
        value += current();
        advance();
        while (isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    // 处理科学计数法
    if (current() == 'e' || current() == 'E') {
        value += current();
        advance();
        if (current() == '+' || current() == '-') {
            value += current();
            advance();
        }
        while (isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    return makeToken(CHTLTokenType::NUMBER, value);
}

CHTLToken CHTLLexer::scanIdentifier() {
    std::string value;
    
    while (isAlphaNumeric(current()) || current() == '_') {
        value += current();
        advance();
    }
    
    // 检查是否是关键字
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return makeToken(it->second, value);
    }
    
    // 检查是否是HTML元素
    auto htmlIt = htmlElements.find(value);
    if (htmlIt != htmlElements.end()) {
        return makeToken(CHTLTokenType::HTML_ELEMENT, value);
    }
    
    return makeToken(CHTLTokenType::IDENTIFIER, value);
}

CHTLToken CHTLLexer::scanOperator() {
    std::string value;
    value += current();
    advance();
    
    // 检查双字符运算符
    if (!isAtEnd()) {
        std::string twoChar = value + current();
        auto it = operators.find(twoChar);
        if (it != operators.end()) {
            advance();
            return makeToken(it->second, twoChar);
        }
    }
    
    // 检查单字符运算符
    auto it = operators.find(value);
    if (it != operators.end()) {
        return makeToken(it->second, value);
    }
    
    return makeToken(CHTLTokenType::UNKNOWN, value);
}

CHTLToken CHTLLexer::scanHTMLElement() {
    return scanIdentifier(); // HTML元素在scanIdentifier中处理
}

CHTLToken CHTLLexer::scanTemplate() {
    advance(); // 跳过 '['
    advance(); // 跳过 'T'
    advance(); // 跳过 'e'
    advance(); // 跳过 'm'
    advance(); // 跳过 'p'
    advance(); // 跳过 'l'
    advance(); // 跳过 'a'
    advance(); // 跳过 't'
    advance(); // 跳过 'e'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::TEMPLATE);
}

CHTLToken CHTLLexer::scanCustom() {
    advance(); // 跳过 '['
    advance(); // 跳过 'C'
    advance(); // 跳过 'u'
    advance(); // 跳过 's'
    advance(); // 跳过 't'
    advance(); // 跳过 'o'
    advance(); // 跳过 'm'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::CUSTOM);
}

CHTLToken CHTLLexer::scanOrigin() {
    advance(); // 跳过 '['
    advance(); // 跳过 'O'
    advance(); // 跳过 'r'
    advance(); // 跳过 'i'
    advance(); // 跳过 'g'
    advance(); // 跳过 'i'
    advance(); // 跳过 'n'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::ORIGIN);
}

CHTLToken CHTLLexer::scanImport() {
    advance(); // 跳过 '['
    advance(); // 跳过 'I'
    advance(); // 跳过 'm'
    advance(); // 跳过 'p'
    advance(); // 跳过 'o'
    advance(); // 跳过 'r'
    advance(); // 跳过 't'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::IMPORT);
}

CHTLToken CHTLLexer::scanNamespace() {
    advance(); // 跳过 '['
    advance(); // 跳过 'N'
    advance(); // 跳过 'a'
    advance(); // 跳过 'm'
    advance(); // 跳过 'e'
    advance(); // 跳过 's'
    advance(); // 跳过 'p'
    advance(); // 跳过 'a'
    advance(); // 跳过 'c'
    advance(); // 跳过 'e'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::NAMESPACE);
}

CHTLToken CHTLLexer::scanConfiguration() {
    advance(); // 跳过 '['
    advance(); // 跳过 'C'
    advance(); // 跳过 'o'
    advance(); // 跳过 'n'
    advance(); // 跳过 'f'
    advance(); // 跳过 'i'
    advance(); // 跳过 'g'
    advance(); // 跳过 'u'
    advance(); // 跳过 'r'
    advance(); // 跳过 'a'
    advance(); // 跳过 't'
    advance(); // 跳过 'i'
    advance(); // 跳过 'o'
    advance(); // 跳过 'n'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::CONFIGURATION);
}

CHTLToken CHTLLexer::scanInfo() {
    advance(); // 跳过 '['
    advance(); // 跳过 'I'
    advance(); // 跳过 'n'
    advance(); // 跳过 'f'
    advance(); // 跳过 'o'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::INFO);
}

CHTLToken CHTLLexer::scanExport() {
    advance(); // 跳过 '['
    advance(); // 跳过 'E'
    advance(); // 跳过 'x'
    advance(); // 跳过 'p'
    advance(); // 跳过 'o'
    advance(); // 跳过 'r'
    advance(); // 跳过 't'
    advance(); // 跳过 ']'
    
    return makeToken(CHTLTokenType::EXPORT);
}

CHTLToken CHTLLexer::scanGeneratorComment() {
    advance(); // 跳过 '#'
    
    std::string value;
    while (!isAtEnd() && !isNewline(current())) {
        value += current();
        advance();
    }
    
    return makeToken(CHTLTokenType::GENERATOR_COMMENT, value);
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
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

bool CHTLLexer::isHTMLChar(char c) const {
    return isAlphaNumeric(c) || c == '-';
}

CHTLToken CHTLLexer::makeToken(CHTLTokenType type, const std::string& value) {
    return CHTLToken(type, value, line, column - value.length(), start);
}

CHTLToken CHTLLexer::makeToken(CHTLTokenType type, char value) {
    return CHTLToken(type, std::string(1, value), line, column - 1, start);
}

void CHTLLexer::error(const std::string& message) const {
    std::ostringstream oss;
    oss << "Lexer error at line " << line << ", column " << column << ": " << message;
    throw std::runtime_error(oss.str());
}

CHTLTokenList CHTLLexer::tokenize() {
    CHTLTokenList tokens;
    position = 0;
    line = 1;
    column = 1;
    
    while (!isAtEnd()) {
        start = position;
        
        skipWhitespace();
        if (isAtEnd()) break;
        
        char c = current();
        
        if (c == '/' && (peek() == '/' || peek() == '*')) {
            skipComment();
            continue;
        }
        
        if (c == '#') {
            tokens.addToken(scanGeneratorComment());
            continue;
        }
        
        if (c == '"' || c == '\'') {
            tokens.addToken(scanString());
            continue;
        }
        
        if (isDigit(c)) {
            tokens.addToken(scanNumber());
            continue;
        }
        
        if (isAlpha(c)) {
            tokens.addToken(scanIdentifier());
            continue;
        }
        
        if (c == '[') {
            // 检查是否是特殊标记
            std::string marker;
            size_t tempPos = position;
            while (tempPos < source.length() && source[tempPos] != ']') {
                marker += source[tempPos];
                tempPos++;
            }
            marker += ']';
            
            if (marker == "[Template]") {
                tokens.addToken(scanTemplate());
                continue;
            } else if (marker == "[Custom]") {
                tokens.addToken(scanCustom());
                continue;
            } else if (marker == "[Origin]") {
                tokens.addToken(scanOrigin());
                continue;
            } else if (marker == "[Import]") {
                tokens.addToken(scanImport());
                continue;
            } else if (marker == "[Namespace]") {
                tokens.addToken(scanNamespace());
                continue;
            } else if (marker == "[Configuration]") {
                tokens.addToken(scanConfiguration());
                continue;
            } else if (marker == "[Info]") {
                tokens.addToken(scanInfo());
                continue;
            } else if (marker == "[Export]") {
                tokens.addToken(scanExport());
                continue;
            }
        }
        
        if (operators.find(std::string(1, c)) != operators.end()) {
            tokens.addToken(scanOperator());
            continue;
        }
        
        // 未知字符
        advance();
        tokens.addToken(makeToken(CHTLTokenType::UNKNOWN, c));
    }
    
    tokens.addToken(CHTLToken(CHTLTokenType::EOF_TOKEN, "", line, column, position));
    return tokens;
}

CHTLTokenList CHTLLexer::tokenize(const std::string& source) {
    setSource(source);
    return tokenize();
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    start = 0;
}

void CHTLLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

std::string CHTLLexer::getSource() const {
    return source;
}

size_t CHTLLexer::getPosition() const {
    return position;
}

size_t CHTLLexer::getLine() const {
    return line;
}

size_t CHTLLexer::getColumn() const {
    return column;
}

} // namespace CHTL