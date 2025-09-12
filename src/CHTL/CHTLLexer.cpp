#include "CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() : position(0), line(1), column(1) {
    initializeKeywords();
    initializeOperators();
}

CHTLLexer::~CHTLLexer() {}

void CHTLLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    reset();
    
    while (!isEOF()) {
        skipWhitespace();
        if (isEOF()) break;
        
        auto token = nextToken();
        if (token) {
            tokens.push_back(*token);
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column, position);
    
    return tokens;
}

std::unique_ptr<Token> CHTLLexer::nextToken() {
    if (isEOF()) {
        return createToken(TokenType::EOF_TOKEN, "");
    }
    
    char c = currentChar();
    
    // 处理注释
    if (c == '/' && nextChar() == '/') {
        skipComment();
        return nextToken();
    }
    
    if (c == '/' && nextChar() == '*') {
        skipComment();
        return nextToken();
    }
    
    if (c == '-' && nextChar() == '-') {
        std::string comment = readLiteral();
        return createToken(TokenType::COMMENT_GEN, comment);
    }
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        std::string str = readString();
        return createToken(TokenType::STRING, str);
    }
    
    // 处理方括号关键字
    if (c == '[') {
        std::string keyword = readBracketKeyword();
        auto it = keywords.find(keyword);
        if (it != keywords.end()) {
            return createToken(it->second, keyword);
        }
    }
    
    // 处理@关键字
    if (c == '@') {
        std::string keyword = readAtKeyword();
        auto it = keywords.find(keyword);
        if (it != keywords.end()) {
            return createToken(it->second, keyword);
        }
    }
    
    // 处理数字
    if (isDigit(c)) {
        std::string num = readNumber();
        return createToken(TokenType::NUMBER, num);
    }
    
    // 处理标识符和关键字
    if (isAlpha(c) || c == '_') {
        std::string identifier = readIdentifier();
        auto it = keywords.find(identifier);
        if (it != keywords.end()) {
            return createToken(it->second, identifier);
        }
        return createToken(TokenType::IDENTIFIER, identifier);
    }
    
    // 处理操作符
    if (isOperatorChar(c)) {
        std::string op = readOperator();
        auto it = operators.find(op);
        if (it != operators.end()) {
            return createToken(it->second, op);
        }
        return createToken(TokenType::UNKNOWN, op);
    }
    
    // 处理分隔符
    switch (c) {
        case '{':
            advance();
            return createToken(TokenType::LEFT_BRACE, "{");
        case '}':
            advance();
            return createToken(TokenType::RIGHT_BRACE, "}");
        case '(':
            advance();
            return createToken(TokenType::LEFT_PAREN, "(");
        case ')':
            advance();
            return createToken(TokenType::RIGHT_PAREN, ")");
        case '[':
            advance();
            return createToken(TokenType::LEFT_BRACKET, "[");
        case ']':
            advance();
            return createToken(TokenType::RIGHT_BRACKET, "]");
        case ';':
            advance();
            return createToken(TokenType::SEMICOLON, ";");
        case ',':
            advance();
            return createToken(TokenType::COMMA, ",");
        case '.':
            advance();
            return createToken(TokenType::DOT, ".");
        case '\n':
            advance();
            line++;
            column = 1;
            return createToken(TokenType::NEWLINE, "\n");
    }
    
    // 未知字符
    advance();
    return createToken(TokenType::UNKNOWN, std::string(1, c));
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
}

size_t CHTLLexer::getCurrentPosition() const {
    return position;
}

size_t CHTLLexer::getCurrentLine() const {
    return line;
}

size_t CHTLLexer::getCurrentColumn() const {
    return column;
}

void CHTLLexer::initializeKeywords() {
    // 基础关键字
    keywords["text"] = TokenType::TEXT;
    keywords["style"] = TokenType::STYLE;
    keywords["script"] = TokenType::SCRIPT;
    
    // 方括号关键字
    keywords["[Template]"] = TokenType::TEMPLATE;
    keywords["[Custom]"] = TokenType::CUSTOM;
    keywords["[Import]"] = TokenType::IMPORT;
    keywords["[Namespace]"] = TokenType::NAMESPACE;
    keywords["[Origin]"] = TokenType::ORIGIN;
    keywords["[Configuration]"] = TokenType::CONFIGURATION;
    keywords["[Info]"] = TokenType::INFO;
    keywords["[Export]"] = TokenType::EXPORT;
    
    // @关键字
    keywords["@Style"] = TokenType::TEMPLATE_STYLE;
    keywords["@Element"] = TokenType::TEMPLATE_ELEMENT;
    keywords["@Var"] = TokenType::TEMPLATE_VAR;
    keywords["@Html"] = TokenType::ORIGIN_HTML;
    keywords["@JavaScript"] = TokenType::ORIGIN_JAVASCRIPT;
    keywords["@Chtl"] = TokenType::IMPORT_CHTL;
    keywords["@CJmod"] = TokenType::IMPORT_CJMOD;
    keywords["@Config"] = TokenType::CONFIGURATION;
    
    // 其他关键字
    keywords["inherit"] = TokenType::INHERIT;
    keywords["delete"] = TokenType::DELETE;
    keywords["insert"] = TokenType::INSERT;
    keywords["after"] = TokenType::AFTER;
    keywords["before"] = TokenType::BEFORE;
    keywords["replace"] = TokenType::REPLACE;
    keywords["at top"] = TokenType::AT_TOP;
    keywords["at bottom"] = TokenType::AT_BOTTOM;
    keywords["from"] = TokenType::FROM;
    keywords["as"] = TokenType::AS;
    keywords["except"] = TokenType::EXCEPT;
    keywords["use"] = TokenType::USE;
    keywords["html5"] = TokenType::HTML5;
}

void CHTLLexer::initializeOperators() {
    // 单字符操作符
    operators[":"] = TokenType::COLON;
    operators["="] = TokenType::EQUAL;
    operators["?"] = TokenType::QUESTION;
    operators["!"] = TokenType::EXCLAMATION;
    operators["&"] = TokenType::AMPERSAND;
    operators["|"] = TokenType::PIPE;
    operators["+"] = TokenType::PLUS;
    operators["-"] = TokenType::MINUS;
    operators["*"] = TokenType::MULTIPLY;
    operators["/"] = TokenType::DIVIDE;
    operators["%"] = TokenType::MODULO;
    operators["<"] = TokenType::LESS;
    operators[">"] = TokenType::GREATER;
    
    // 双字符操作符
    operators["->"] = TokenType::ARROW;
    operators["**"] = TokenType::POWER;
    operators["<="] = TokenType::LESS_EQUAL;
    operators[">="] = TokenType::GREATER_EQUAL;
    operators["=="] = TokenType::EQUAL_EQUAL;
    operators["!="] = TokenType::NOT_EQUAL;
    operators["&&"] = TokenType::AND;
    operators["||"] = TokenType::OR;
}

void CHTLLexer::skipWhitespace() {
    while (!isEOF() && isWhitespace(currentChar()) && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipComment() {
    char c = currentChar();
    char next = nextChar();
    
    if (c == '/' && next == '/') {
        // 单行注释
        advance(); // 跳过第一个 '/'
        advance(); // 跳过第二个 '/'
        while (!isEOF() && currentChar() != '\n') {
            advance();
        }
    } else if (c == '/' && next == '*') {
        // 多行注释
        advance(); // 跳过 '/'
        advance(); // 跳过 '*'
        while (!isEOF()) {
            if (currentChar() == '*' && nextChar() == '/') {
                advance(); // 跳过 '*'
                advance(); // 跳过 '/'
                break;
            }
            advance();
        }
    }
}

std::string CHTLLexer::readIdentifier() {
    std::string result;
    while (!isEOF() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readString() {
    std::string result;
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    while (!isEOF() && currentChar() != quote) {
        if (currentChar() == '\\' && nextChar() == quote) {
            advance(); // 跳过反斜杠
            result += currentChar();
            advance();
        } else {
            result += currentChar();
            advance();
        }
    }
    
    if (!isEOF()) {
        advance(); // 跳过结束引号
    }
    
    return result;
}

std::string CHTLLexer::readLiteral() {
    std::string result;
    while (!isEOF() && !isWhitespace(currentChar()) && 
           currentChar() != ';' && currentChar() != ':' && 
           currentChar() != '=' && currentChar() != '{' && 
           currentChar() != '}' && currentChar() != '\n') {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readNumber() {
    std::string result;
    while (!isEOF() && (isDigit(currentChar()) || currentChar() == '.')) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readOperator() {
    std::string result;
    char c = currentChar();
    char next = nextChar();
    
    // 检查双字符操作符
    if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
        (c == '<' && next == '=') || (c == '>' && next == '=') ||
        (c == '&' && next == '&') || (c == '|' && next == '|') ||
        (c == '*' && next == '*') || (c == '-' && next == '>')) {
        result += c;
        advance();
        result += currentChar();
        advance();
    } else {
        result += c;
        advance();
    }
    
    return result;
}

std::string CHTLLexer::readBracketKeyword() {
    std::string result;
    result += currentChar(); // '['
    advance();
    
    while (!isEOF() && currentChar() != ']') {
        result += currentChar();
        advance();
    }
    
    if (!isEOF()) {
        result += currentChar(); // ']'
        advance();
    }
    
    return result;
}

std::string CHTLLexer::readAtKeyword() {
    std::string result;
    result += currentChar(); // '@'
    advance();
    
    while (!isEOF() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        result += currentChar();
        advance();
    }
    
    return result;
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

bool CHTLLexer::isOperatorChar(char c) const {
    return c == ':' || c == '=' || c == '?' || c == '!' || c == '&' || 
           c == '|' || c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '%' || c == '<' || c == '>';
}

char CHTLLexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLLexer::nextChar() const {
    if (position + 1 >= source.length()) {
        return '\0';
    }
    return source[position + 1];
}

void CHTLLexer::advance() {
    if (position < source.length()) {
        position++;
        column++;
    }
}

void CHTLLexer::retreat() {
    if (position > 0) {
        position--;
        column--;
    }
}

bool CHTLLexer::isEOF() const {
    return position >= source.length();
}

std::unique_ptr<Token> CHTLLexer::createToken(TokenType type, const std::string& value) {
    return std::make_unique<Token>(type, value, line, column, position);
}

// Token类方法实现
bool Token::isKeyword() const {
    return type >= TokenType::TEXT && type <= TokenType::HTML5;
}

bool Token::isOperator() const {
    return type >= TokenType::COLON && type <= TokenType::OR;
}

bool Token::isDelimiter() const {
    return type >= TokenType::LEFT_BRACE && type <= TokenType::DOT;
}

std::string Token::toString() const {
    std::string typeStr;
    switch (type) {
        case TokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case TokenType::STRING: typeStr = "STRING"; break;
        case TokenType::NUMBER: typeStr = "NUMBER"; break;
        case TokenType::LITERAL: typeStr = "LITERAL"; break;
        case TokenType::LEFT_BRACE: typeStr = "LEFT_BRACE"; break;
        case TokenType::RIGHT_BRACE: typeStr = "RIGHT_BRACE"; break;
        case TokenType::COLON: typeStr = "COLON"; break;
        case TokenType::EQUAL: typeStr = "EQUAL"; break;
        case TokenType::TEXT: typeStr = "TEXT"; break;
        case TokenType::STYLE: typeStr = "STYLE"; break;
        case TokenType::SCRIPT: typeStr = "SCRIPT"; break;
        case TokenType::TEMPLATE: typeStr = "TEMPLATE"; break;
        case TokenType::CUSTOM: typeStr = "CUSTOM"; break;
        case TokenType::IMPORT: typeStr = "IMPORT"; break;
        case TokenType::NAMESPACE: typeStr = "NAMESPACE"; break;
        case TokenType::ORIGIN: typeStr = "ORIGIN"; break;
        case TokenType::CONFIGURATION: typeStr = "CONFIGURATION"; break;
        case TokenType::EOF_TOKEN: typeStr = "EOF"; break;
        default: typeStr = "UNKNOWN"; break;
    }
    
    return "Token(" + typeStr + ", \"" + value + "\", " + 
           std::to_string(line) + ":" + std::to_string(column) + ")";
}

} // namespace CHTL