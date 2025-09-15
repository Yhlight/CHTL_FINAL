#include "CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() : position(0), line(1), column(1) {
}

CHTLLexer::~CHTLLexer() {
}

std::vector<CHTLToken> CHTLLexer::tokenize(const std::string& source) {
    setSource(source);
    std::vector<CHTLToken> tokens;
    
    while (hasMoreTokens()) {
        CHTLToken token = nextToken();
        if (token.getType() != CHTLTokenType::WHITESPACE && 
            token.getType() != CHTLTokenType::COMMENT) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.push_back(CHTLToken(CHTLTokenType::EOF_TOKEN, "", line, column));
    
    return tokens;
}

void CHTLLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    errors.clear();
}

bool CHTLLexer::hasMoreTokens() const {
    return position < source.length();
}

CHTLToken CHTLLexer::nextToken() {
    if (isAtEnd()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN, "", line, column);
    }
    
    skipWhitespace();
    
    if (isAtEnd()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN, "", line, column);
    }
    
    char c = currentChar();
    
    // 生成器注释
    if (c == '#') {
        return readGeneratorComment();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return readString();
    }
    
    // 数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 操作符
    if (isOperatorChar(c)) {
        return readOperator();
    }
    
    // 标点符号
    if (isPunctuationChar(c)) {
        return readPunctuation();
    }
    
    // 特殊符号
    if (c == '$' || c == '@' || c == '#' || c == '%' || c == '|' || c == '&') {
        return readSpecial();
    }
    
    // 注释
    if (c == '/' && peekChar() == '/') {
        skipComment();
        return nextToken();
    }
    
    // 未知字符
    reportError("Unknown character: " + std::string(1, c));
    advance();
    return CHTLToken(CHTLTokenType::UNKNOWN, std::string(1, c), line, column);
}

CHTLToken CHTLLexer::peekToken() {
    size_t oldPosition = position;
    size_t oldLine = line;
    size_t oldColumn = column;
    
    CHTLToken token = nextToken();
    
    position = oldPosition;
    line = oldLine;
    column = oldColumn;
    
    return token;
}

std::vector<std::string> CHTLLexer::getErrors() const {
    return errors;
}

bool CHTLLexer::hasErrors() const {
    return !errors.empty();
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        advance();
    }
}

void CHTLLexer::skipComment() {
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

CHTLToken CHTLLexer::readString() {
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
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
            }
        } else {
            value += currentChar();
        }
        advance();
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string literal");
        return CHTLToken(CHTLTokenType::STRING, value, line, column);
    }
    
    advance(); // 跳过结束引号
    return CHTLToken(CHTLTokenType::STRING, value, line, column);
}

CHTLToken CHTLLexer::readNumber() {
    std::string value;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return CHTLToken(CHTLTokenType::NUMBER, value, line, column);
}

CHTLToken CHTLLexer::readIdentifier() {
    std::string value;
    
    while (!isAtEnd() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    
    CHTLTokenType type = getKeywordType(value);
    return CHTLToken(type, value, line, column);
}

CHTLToken CHTLLexer::readOperator() {
    char c = currentChar();
    advance();
    
    switch (c) {
        case ':': return CHTLToken(CHTLTokenType::COLON, ":", line, column);
        case '=': return CHTLToken(CHTLTokenType::EQUAL, "=", line, column);
        case ';': return CHTLToken(CHTLTokenType::SEMICOLON, ";", line, column);
        case ',': return CHTLToken(CHTLTokenType::COMMA, ",", line, column);
        case '.': return CHTLToken(CHTLTokenType::DOT, ".", line, column);
        case '#': return CHTLToken(CHTLTokenType::HASH, "#", line, column);
        case '%': return CHTLToken(CHTLTokenType::PERCENT, "%", line, column);
        case '&': return CHTLToken(CHTLTokenType::AMPERSAND, "&", line, column);
        case '|': return CHTLToken(CHTLTokenType::PIPE, "|", line, column);
        case '?': return CHTLToken(CHTLTokenType::QUESTION, "?", line, column);
        case '!': return CHTLToken(CHTLTokenType::EXCLAMATION, "!", line, column);
        case '_': return CHTLToken(CHTLTokenType::UNDERSCORE, "_", line, column);
        default: return CHTLToken(CHTLTokenType::UNKNOWN, std::string(1, c), line, column);
    }
}

CHTLToken CHTLLexer::readPunctuation() {
    char c = currentChar();
    advance();
    
    switch (c) {
        case '(': return CHTLToken(CHTLTokenType::LEFT_PAREN, "(", line, column);
        case ')': return CHTLToken(CHTLTokenType::RIGHT_PAREN, ")", line, column);
        case '{': return CHTLToken(CHTLTokenType::LEFT_BRACE, "{", line, column);
        case '}': return CHTLToken(CHTLTokenType::RIGHT_BRACE, "}", line, column);
        case '[': return CHTLToken(CHTLTokenType::LEFT_BRACKET, "[", line, column);
        case ']': return CHTLToken(CHTLTokenType::RIGHT_BRACKET, "]", line, column);
        case '<': return CHTLToken(CHTLTokenType::LEFT_ANGLE, "<", line, column);
        case '>': return CHTLToken(CHTLTokenType::RIGHT_ANGLE, ">", line, column);
        default: return CHTLToken(CHTLTokenType::UNKNOWN, std::string(1, c), line, column);
    }
}

CHTLToken CHTLLexer::readSpecial() {
    char c = currentChar();
    advance();
    
    switch (c) {
        case '$': return CHTLToken(CHTLTokenType::DOLLAR, "$", line, column);
        case '@': return CHTLToken(CHTLTokenType::AT, "@", line, column);
        case '#': return CHTLToken(CHTLTokenType::HASH, "#", line, column);
        case '%': return CHTLToken(CHTLTokenType::PERCENT, "%", line, column);
        case '|': return CHTLToken(CHTLTokenType::PIPE, "|", line, column);
        case '&': return CHTLToken(CHTLTokenType::AMPERSAND, "&", line, column);
        default: return CHTLToken(CHTLTokenType::UNKNOWN, std::string(1, c), line, column);
    }
}

CHTLToken CHTLLexer::readGeneratorComment() {
    std::string value;
    
    while (!isAtEnd() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    
    return CHTLToken(CHTLTokenType::GENERATOR_COMMENT, value, line, column);
}

char CHTLLexer::currentChar() const {
    return isAtEnd() ? '\0' : source[position];
}

char CHTLLexer::peekChar() const {
    return (position + 1 >= source.length()) ? '\0' : source[position + 1];
}

void CHTLLexer::advance() {
    if (!isAtEnd()) {
        position++;
        column++;
    }
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c) || c == '_';
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLLexer::isOperatorChar(char c) const {
    return c == ':' || c == '=' || c == ';' || c == ',' || 
           c == '.' || c == '#' || c == '%' || c == '&' || 
           c == '|' || c == '?' || c == '!' || c == '_';
}

bool CHTLLexer::isPunctuationChar(char c) const {
    return c == '(' || c == ')' || c == '{' || c == '}' || 
           c == '[' || c == ']' || c == '<' || c == '>';
}

CHTLTokenType CHTLLexer::getKeywordType(const std::string& word) const {
    // 首先检查HTML标签
    CHTLTokenType htmlType = getHTMLTagType(word);
    if (htmlType != CHTLTokenType::IDENTIFIER) {
        return htmlType;
    }
    
    // 然后检查CHTL关键字
    CHTLTokenType chtlType = getCHTLKeywordType(word);
    if (chtlType != CHTLTokenType::IDENTIFIER) {
        return chtlType;
    }
    
    // 最后检查其他关键字
    if (word == "text") return CHTLTokenType::TEXT;
    if (word == "inherit") return CHTLTokenType::INHERIT;
    if (word == "delete") return CHTLTokenType::DELETE;
    if (word == "insert") return CHTLTokenType::INSERT;
    if (word == "after") return CHTLTokenType::AFTER;
    if (word == "before") return CHTLTokenType::BEFORE;
    if (word == "replace") return CHTLTokenType::REPLACE;
    if (word == "at") return CHTLTokenType::AT_TOP; // 需要特殊处理
    if (word == "from") return CHTLTokenType::FROM;
    if (word == "as") return CHTLTokenType::AS;
    if (word == "except") return CHTLTokenType::EXCEPT;
    if (word == "use") return CHTLTokenType::USE;
    if (word == "html5") return CHTLTokenType::HTML5;
    if (word == "true" || word == "false") return CHTLTokenType::BOOLEAN;
    
    return CHTLTokenType::IDENTIFIER;
}

CHTLTokenType CHTLLexer::getHTMLTagType(const std::string& word) const {
    if (word == "html") return CHTLTokenType::HTML;
    if (word == "head") return CHTLTokenType::HEAD;
    if (word == "body") return CHTLTokenType::BODY;
    if (word == "title") return CHTLTokenType::TITLE;
    if (word == "div") return CHTLTokenType::DIV;
    if (word == "span") return CHTLTokenType::SPAN;
    if (word == "p") return CHTLTokenType::P;
    if (word == "a") return CHTLTokenType::A;
    if (word == "img") return CHTLTokenType::IMG;
    if (word == "br") return CHTLTokenType::BR;
    if (word == "hr") return CHTLTokenType::HR;
    if (word == "ul") return CHTLTokenType::UL;
    if (word == "ol") return CHTLTokenType::OL;
    if (word == "li") return CHTLTokenType::LI;
    if (word == "table") return CHTLTokenType::TABLE;
    if (word == "tr") return CHTLTokenType::TR;
    if (word == "td") return CHTLTokenType::TD;
    if (word == "th") return CHTLTokenType::TH;
    if (word == "form") return CHTLTokenType::FORM;
    if (word == "input") return CHTLTokenType::INPUT;
    if (word == "button") return CHTLTokenType::BUTTON;
    if (word == "textarea") return CHTLTokenType::TEXTAREA;
    if (word == "select") return CHTLTokenType::SELECT;
    if (word == "option") return CHTLTokenType::OPTION;
    if (word == "script") return CHTLTokenType::SCRIPT;
    if (word == "style") return CHTLTokenType::STYLE;
    if (word == "link") return CHTLTokenType::LINK;
    if (word == "meta") return CHTLTokenType::META;
    
    return CHTLTokenType::IDENTIFIER;
}

CHTLTokenType CHTLLexer::getCHTLKeywordType(const std::string& word) const {
    if (word == "Template") return CHTLTokenType::TEMPLATE;
    if (word == "Custom") return CHTLTokenType::CUSTOM;
    if (word == "Origin") return CHTLTokenType::ORIGIN;
    if (word == "Import") return CHTLTokenType::IMPORT;
    if (word == "Configuration") return CHTLTokenType::CONFIGURATION;
    if (word == "Namespace") return CHTLTokenType::NAMESPACE;
    if (word == "Info") return CHTLTokenType::INFO;
    if (word == "Export") return CHTLTokenType::EXPORT;
    
    return CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isKeyword(const std::string& word) const {
    return getKeywordType(word) != CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isHTMLTag(const std::string& word) const {
    return getHTMLTagType(word) != CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isCHTLKeyword(const std::string& word) const {
    return getCHTLKeywordType(word) != CHTLTokenType::IDENTIFIER;
}

void CHTLLexer::reportError(const std::string& message) {
    std::string error = "Error at line " + std::to_string(line) + 
                       ", column " + std::to_string(column) + ": " + message;
    errors.push_back(error);
}

} // namespace CHTL