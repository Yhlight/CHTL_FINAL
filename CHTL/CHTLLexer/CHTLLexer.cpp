#include "CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

// 静态成员初始化
const std::map<std::string, TokenType> CHTLLexer::keywords_ = {
    {"use", TokenType::USE},
    {"except", TokenType::EXCEPT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"inherit", TokenType::INHERIT},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"at", TokenType::AT},
    {"top", TokenType::TOP},
    {"bottom", TokenType::BOTTOM},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"html5", TokenType::HTML_ELEMENT}
};

const std::map<std::string, TokenType> CHTLLexer::html_elements_ = {
    {"html", TokenType::HTML_ELEMENT},
    {"head", TokenType::HTML_ELEMENT},
    {"body", TokenType::HTML_ELEMENT},
    {"title", TokenType::HTML_ELEMENT},
    {"meta", TokenType::HTML_ELEMENT},
    {"link", TokenType::HTML_ELEMENT},
    {"script", TokenType::HTML_ELEMENT},
    {"style", TokenType::HTML_ELEMENT},
    {"div", TokenType::HTML_ELEMENT},
    {"span", TokenType::HTML_ELEMENT},
    {"p", TokenType::HTML_ELEMENT},
    {"h1", TokenType::HTML_ELEMENT},
    {"h2", TokenType::HTML_ELEMENT},
    {"h3", TokenType::HTML_ELEMENT},
    {"h4", TokenType::HTML_ELEMENT},
    {"h5", TokenType::HTML_ELEMENT},
    {"h6", TokenType::HTML_ELEMENT},
    {"ul", TokenType::HTML_ELEMENT},
    {"ol", TokenType::HTML_ELEMENT},
    {"li", TokenType::HTML_ELEMENT},
    {"a", TokenType::HTML_ELEMENT},
    {"img", TokenType::HTML_ELEMENT},
    {"button", TokenType::HTML_ELEMENT},
    {"input", TokenType::HTML_ELEMENT},
    {"form", TokenType::HTML_ELEMENT},
    {"label", TokenType::HTML_ELEMENT},
    {"table", TokenType::HTML_ELEMENT},
    {"tr", TokenType::HTML_ELEMENT},
    {"td", TokenType::HTML_ELEMENT},
    {"th", TokenType::HTML_ELEMENT},
    {"thead", TokenType::HTML_ELEMENT},
    {"tbody", TokenType::HTML_ELEMENT},
    {"tfoot", TokenType::HTML_ELEMENT},
    {"section", TokenType::HTML_ELEMENT},
    {"article", TokenType::HTML_ELEMENT},
    {"header", TokenType::HTML_ELEMENT},
    {"footer", TokenType::HTML_ELEMENT},
    {"nav", TokenType::HTML_ELEMENT},
    {"aside", TokenType::HTML_ELEMENT},
    {"main", TokenType::HTML_ELEMENT},
    {"figure", TokenType::HTML_ELEMENT},
    {"figcaption", TokenType::HTML_ELEMENT},
    {"blockquote", TokenType::HTML_ELEMENT},
    {"code", TokenType::HTML_ELEMENT},
    {"pre", TokenType::HTML_ELEMENT},
    {"em", TokenType::HTML_ELEMENT},
    {"strong", TokenType::HTML_ELEMENT},
    {"small", TokenType::HTML_ELEMENT},
    {"mark", TokenType::HTML_ELEMENT},
    {"del", TokenType::HTML_ELEMENT},
    {"ins", TokenType::HTML_ELEMENT},
    {"sub", TokenType::HTML_ELEMENT},
    {"sup", TokenType::HTML_ELEMENT},
    {"br", TokenType::HTML_ELEMENT},
    {"hr", TokenType::HTML_ELEMENT}
};

const std::map<std::string, TokenType> CHTLLexer::template_types_ = {
    {"@Style", TokenType::TEMPLATE_STYLE},
    {"@Element", TokenType::TEMPLATE_ELEMENT},
    {"@Var", TokenType::TEMPLATE_VAR}
};

const std::map<std::string, TokenType> CHTLLexer::custom_types_ = {
    {"@Style", TokenType::CUSTOM_STYLE},
    {"@Element", TokenType::CUSTOM_ELEMENT},
    {"@Var", TokenType::CUSTOM_VAR}
};

const std::map<std::string, TokenType> CHTLLexer::origin_types_ = {
    {"@Html", TokenType::ORIGIN_HTML},
    {"@Style", TokenType::ORIGIN_STYLE},
    {"@JavaScript", TokenType::ORIGIN_JAVASCRIPT}
};

const std::map<std::string, TokenType> CHTLLexer::import_types_ = {
    {"@Html", TokenType::IMPORT_HTML},
    {"@Style", TokenType::IMPORT_STYLE},
    {"@JavaScript", TokenType::IMPORT_JAVASCRIPT},
    {"@Chtl", TokenType::IMPORT_CHTL},
    {"@CJmod", TokenType::IMPORT_CJMOD}
};

CHTLLexer::CHTLLexer(const std::string& source)
    : source_(source), position_(0), line_(1), column_(1) {
}

CHTLLexer::~CHTLLexer() {
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (hasMoreTokens()) {
        Token token = nextToken();
        if (token.getType() != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

Token CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (!hasMoreTokens()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_);
    }
    
    char c = currentChar();
    
    // 生成器注释 #
    if (c == '#') {
        return parseGeneratorComment();
    }
    
    // 行注释 //
    if (c == '/' && peekChar() == '/') {
        skipComment();
        return nextToken();
    }
    
    // 块注释 /* */
    if (c == '/' && peekChar() == '*') {
        skipComment();
        return nextToken();
    }
    
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
    
    // 模板 [Template]
    if (c == '[' && source_.substr(position_, 9) == "[Template]") {
        return parseTemplate();
    }
    
    // 自定义 [Custom]
    if (c == '[' && source_.substr(position_, 7) == "[Custom]") {
        return parseCustom();
    }
    
    // 原始嵌入 [Origin]
    if (c == '[' && source_.substr(position_, 7) == "[Origin]") {
        return parseOrigin();
    }
    
    // 导入 [Import]
    if (c == '[' && source_.substr(position_, 7) == "[Import]") {
        return parseImport();
    }
    
    // 命名空间 [Namespace]
    if (c == '[' && source_.substr(position_, 10) == "[Namespace]") {
        return parseNamespace();
    }
    
    // 配置 [Configuration]
    if (c == '[' && source_.substr(position_, 13) == "[Configuration]") {
        return parseConfiguration();
    }
    
    // 信息 [Info]
    if (c == '[' && source_.substr(position_, 5) == "[Info]") {
        advance(); // skip [
        advance(); // skip I
        advance(); // skip n
        advance(); // skip f
        advance(); // skip o
        advance(); // skip ]
        return Token(TokenType::INFO, "[Info]", line_, column_ - 6);
    }
    
    // 导出 [Export]
    if (c == '[' && source_.substr(position_, 7) == "[Export]") {
        advance(); // skip [
        advance(); // skip E
        advance(); // skip x
        advance(); // skip p
        advance(); // skip o
        advance(); // skip r
        advance(); // skip t
        advance(); // skip ]
        return Token(TokenType::EXPORT, "[Export]", line_, column_ - 8);
    }
    
    // 标识符和关键字
    if (isLetter(c) || c == '_') {
        return parseIdentifier();
    }
    
    // HTML元素
    if (isLetter(c)) {
        return parseHTML();
    }
    
    // 操作符和符号
    switch (c) {
        case ':':
            advance();
            return Token(TokenType::COLON, ":", line_, column_);
            
        case '=':
            advance();
            return Token(TokenType::EQUAL, "=", line_, column_);
            
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", line_, column_);
            
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", line_, column_);
            
        case '.':
            advance();
            return Token(TokenType::DOT, ".", line_, column_);
            
        case '?':
            advance();
            return Token(TokenType::QUESTION, "?", line_, column_);
            
        case '!':
            advance();
            return Token(TokenType::EXCLAMATION, "!", line_, column_);
            
        case '>':
            advance();
            if (currentChar() == '-') {
                advance();
                return Token(TokenType::ARROW, "->", line_, column_ - 1);
            }
            return Token(TokenType::RIGHT_ANGLE, ">", line_, column_);
            
        case '&':
            advance();
            if (currentChar() == '-' && peekChar() == '>') {
                advance();
                advance();
                return Token(TokenType::BIND_OP, "&->", line_, column_ - 2);
            }
            return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_);
            
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
            
        case '<':
            advance();
            return Token(TokenType::LEFT_ANGLE, "<", line_, column_);
            
        case '\n':
            advance();
            return Token(TokenType::NEWLINE, "\n", line_, column_);
            
        default:
            advance();
            return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_);
    }
}

bool CHTLLexer::hasMoreTokens() const {
    return position_ < source_.length();
}

int CHTLLexer::getCurrentLine() const {
    return line_;
}

int CHTLLexer::getCurrentColumn() const {
    return column_;
}

const std::string& CHTLLexer::getSource() const {
    return source_;
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

void CHTLLexer::advance() {
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

void CHTLLexer::skipWhitespace() {
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLLexer::skipComment() {
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

Token CHTLLexer::parseString() {
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

Token CHTLLexer::parseNumber() {
    std::string value;
    
    while (hasMoreTokens() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, line_, column_);
}

Token CHTLLexer::parseIdentifier() {
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

Token CHTLLexer::parseHTML() {
    std::string value;
    
    while (hasMoreTokens() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::HTML_ELEMENT, value, line_, column_);
}

Token CHTLLexer::parseTemplate() {
    advance(); // skip [
    advance(); // skip T
    advance(); // skip e
    advance(); // skip m
    advance(); // skip p
    advance(); // skip l
    advance(); // skip a
    advance(); // skip t
    advance(); // skip e
    advance(); // skip ]
    
    return Token(TokenType::TEMPLATE, "[Template]", line_, column_ - 10);
}

Token CHTLLexer::parseCustom() {
    advance(); // skip [
    advance(); // skip C
    advance(); // skip u
    advance(); // skip s
    advance(); // skip t
    advance(); // skip o
    advance(); // skip m
    advance(); // skip ]
    
    return Token(TokenType::CUSTOM, "[Custom]", line_, column_ - 8);
}

Token CHTLLexer::parseOrigin() {
    advance(); // skip [
    advance(); // skip O
    advance(); // skip r
    advance(); // skip i
    advance(); // skip g
    advance(); // skip i
    advance(); // skip n
    advance(); // skip ]
    
    return Token(TokenType::ORIGIN, "[Origin]", line_, column_ - 8);
}

Token CHTLLexer::parseImport() {
    advance(); // skip [
    advance(); // skip I
    advance(); // skip m
    advance(); // skip p
    advance(); // skip o
    advance(); // skip r
    advance(); // skip t
    advance(); // skip ]
    
    return Token(TokenType::IMPORT, "[Import]", line_, column_ - 8);
}

Token CHTLLexer::parseNamespace() {
    advance(); // skip [
    advance(); // skip N
    advance(); // skip a
    advance(); // skip m
    advance(); // skip e
    advance(); // skip s
    advance(); // skip p
    advance(); // skip a
    advance(); // skip c
    advance(); // skip e
    advance(); // skip ]
    
    return Token(TokenType::NAMESPACE, "[Namespace]", line_, column_ - 11);
}

Token CHTLLexer::parseConfiguration() {
    advance(); // skip [
    advance(); // skip C
    advance(); // skip o
    advance(); // skip n
    advance(); // skip f
    advance(); // skip i
    advance(); // skip g
    advance(); // skip u
    advance(); // skip r
    advance(); // skip a
    advance(); // skip t
    advance(); // skip i
    advance(); // skip o
    advance(); // skip n
    advance(); // skip ]
    
    return Token(TokenType::CONFIGURATION, "[Configuration]", line_, column_ - 15);
}

Token CHTLLexer::parseSelector() {
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

Token CHTLLexer::parseResponsiveValue() {
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

Token CHTLLexer::parseGeneratorComment() {
    advance(); // skip #
    
    std::string value;
    while (hasMoreTokens() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, line_, column_);
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
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

TokenType CHTLLexer::getKeywordType(const std::string& keyword) const {
    auto it = keywords_.find(keyword);
    if (it != keywords_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getHTMLType(const std::string& element) const {
    auto it = html_elements_.find(element);
    if (it != html_elements_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getTemplateType(const std::string& type) const {
    auto it = template_types_.find(type);
    if (it != template_types_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getCustomType(const std::string& type) const {
    auto it = custom_types_.find(type);
    if (it != custom_types_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getOriginType(const std::string& type) const {
    auto it = origin_types_.find(type);
    if (it != origin_types_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getImportType(const std::string& type) const {
    auto it = import_types_.find(type);
    if (it != import_types_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

} // namespace CHTL