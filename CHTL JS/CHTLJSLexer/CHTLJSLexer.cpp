#include "CHTLJSLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTLJS {

// 静态成员初始化
const std::map<std::string, CHTLJS_TokenType> CHTLJSLexer::keywords_ = {
    // CHTL JS特有关键字
    {"ScriptLoader", CHTLJS_TokenType::SCRIPT_LOADER},
    {"Listen", CHTLJS_TokenType::LISTEN},
    {"Animate", CHTLJS_TokenType::ANIMATE},
    {"Router", CHTLJS_TokenType::ROUTER},
    {"Vir", CHTLJS_TokenType::VIR},
    {"util", CHTLJS_TokenType::UTIL},
    {"change", CHTLJS_TokenType::CHANGE},
    {"then", CHTLJS_TokenType::THEN},
    {"printMylove", CHTLJS_TokenType::PRINTMYLOVE},
    {"iNeverAway", CHTLJS_TokenType::INEVERAWAY},
    
    // 布尔字面量
    {"true", CHTLJS_TokenType::BOOLEAN_LITERAL},
    {"false", CHTLJS_TokenType::BOOLEAN_LITERAL}
};

const std::map<std::string, CHTLJS_TokenType> CHTLJSLexer::operators_ = {
    // CHTL JS特有操作符
    {"&->", CHTLJS_TokenType::BIND_OPERATOR},
    {"->", CHTLJS_TokenType::ARROW_OPERATOR},
    
    // 传统操作符
    {"=", CHTLJS_TokenType::ASSIGN},
    {"==", CHTLJS_TokenType::EQUAL},
    {"!=", CHTLJS_TokenType::NOT_EQUAL},
    {"<", CHTLJS_TokenType::LESS},
    {">", CHTLJS_TokenType::GREATER},
    {"<=", CHTLJS_TokenType::LESS_EQUAL},
    {">=", CHTLJS_TokenType::GREATER_EQUAL},
    {"&&", CHTLJS_TokenType::AND},
    {"||", CHTLJS_TokenType::OR},
    {"!", CHTLJS_TokenType::NOT},
    {"+", CHTLJS_TokenType::PLUS},
    {"-", CHTLJS_TokenType::MINUS},
    {"*", CHTLJS_TokenType::MULTIPLY},
    {"/", CHTLJS_TokenType::DIVIDE},
    {"%", CHTLJS_TokenType::MODULO},
    {"**", CHTLJS_TokenType::POWER}
};

const std::map<std::string, CHTLJS_TokenType> CHTLJSLexer::punctuation_ = {
    {"(", CHTLJS_TokenType::LEFT_PAREN},
    {")", CHTLJS_TokenType::RIGHT_PAREN},
    {"{", CHTLJS_TokenType::LEFT_BRACE},
    {"}", CHTLJS_TokenType::RIGHT_BRACE},
    {"[", CHTLJS_TokenType::LEFT_BRACKET},
    {"]", CHTLJS_TokenType::RIGHT_BRACKET},
    {";", CHTLJS_TokenType::SEMICOLON},
    {",", CHTLJS_TokenType::COMMA},
    {".", CHTLJS_TokenType::DOT},
    {":", CHTLJS_TokenType::COLON},
    {"?", CHTLJS_TokenType::QUESTION}
};

CHTLJSLexer::CHTLJSLexer(const std::string& source)
    : source_(source), position_(0), line_(1), column_(1) {
}

CHTLJSLexer::~CHTLJSLexer() {
}

std::vector<CHTLJS_Token> CHTLJSLexer::tokenize() {
    std::vector<CHTLJS_Token> tokens;
    
    while (hasMoreTokens()) {
        CHTLJS_Token token = nextToken();
        if (token.type != CHTLJS_TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

CHTLJS_Token CHTLJSLexer::nextToken() {
    skipWhitespace();
    
    if (!hasMoreTokens()) {
        return CHTLJS_Token(CHTLJS_TokenType::EOF_TOKEN, "", line_, column_);
    }
    
    char c = currentChar();
    
    // CHTL选择器 {{...}}
    if (c == '{' && peekChar() == '{') {
        return parseCHTLSelector();
    }
    
    // CHTL响应式值 $JS变量名$
    if (c == '$') {
        return parseCHTLResponsive();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return parseString();
    }
    
    // 数字
    if (isDigit(c)) {
        return parseNumber();
    }
    
    // 标识符和关键字
    if (isLetter(c) || c == '_') {
        return parseIdentifier();
    }
    
    // 操作符和标点符号
    if (isOperatorChar(c)) {
        return parseOperator();
    }
    
    // 标点符号
    if (isPunctuationChar(c)) {
        return parsePunctuation();
    }
    
    // 换行
    if (c == '\n') {
        advance();
        return CHTLJS_Token(CHTLJS_TokenType::NEWLINE, "\n", line_, column_);
    }
    
    // 未知字符
    advance();
    return CHTLJS_Token(CHTLJS_TokenType::UNKNOWN, std::string(1, c), line_, column_);
}

bool CHTLJSLexer::hasMoreTokens() const {
    return position_ < source_.length();
}

int CHTLJSLexer::getCurrentLine() const {
    return line_;
}

int CHTLJSLexer::getCurrentColumn() const {
    return column_;
}

const std::string& CHTLJSLexer::getSource() const {
    return source_;
}

char CHTLJSLexer::currentChar() const {
    if (position_ >= source_.length()) {
        return '\0';
    }
    return source_[position_];
}

char CHTLJSLexer::peekChar() const {
    if (position_ + 1 >= source_.length()) {
        return '\0';
    }
    return source_[position_ + 1];
}

void CHTLJSLexer::advance() {
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

void CHTLJSLexer::skipWhitespace() {
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLJSLexer::skipComment() {
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

CHTLJS_Token CHTLJSLexer::parseString() {
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
    
    return CHTLJS_Token(CHTLJS_TokenType::STRING_LITERAL, value, line_, column_);
}

CHTLJS_Token CHTLJSLexer::parseNumber() {
    std::string value;
    
    while (hasMoreTokens() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return CHTLJS_Token(CHTLJS_TokenType::NUMBER_LITERAL, value, line_, column_);
}

CHTLJS_Token CHTLJSLexer::parseIdentifier() {
    std::string value;
    
    while (hasMoreTokens() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    
    CHTLJS_TokenType type = getKeywordType(value);
    if (type != CHTLJS_TokenType::UNKNOWN) {
        return CHTLJS_Token(type, value, line_, column_);
    }
    
    return CHTLJS_Token(CHTLJS_TokenType::IDENTIFIER, value, line_, column_);
}

CHTLJS_Token CHTLJSLexer::parseCHTLSelector() {
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
    
    return CHTLJS_Token(CHTLJS_TokenType::CHTL_SELECTOR, value, line_, column_);
}

CHTLJS_Token CHTLJSLexer::parseCHTLResponsive() {
    advance(); // skip $
    
    std::string value;
    while (hasMoreTokens() && currentChar() != '$') {
        value += currentChar();
        advance();
    }
    
    if (hasMoreTokens()) {
        advance(); // skip closing $
    }
    
    return CHTLJS_Token(CHTLJS_TokenType::CHTL_RESPONSIVE, value, line_, column_);
}

CHTLJS_Token CHTLJSLexer::parseOperator() {
    std::string value;
    value += currentChar();
    advance();
    
    // 检查多字符操作符
    if (hasMoreTokens()) {
        std::string twoChar = value + currentChar();
        if (operators_.find(twoChar) != operators_.end()) {
            value = twoChar;
            advance();
        }
    }
    
    CHTLJS_TokenType type = getOperatorType(value);
    if (type != CHTLJS_TokenType::UNKNOWN) {
        return CHTLJS_Token(type, value, line_, column_);
    }
    
    return CHTLJS_Token(CHTLJS_TokenType::UNKNOWN, value, line_, column_);
}

CHTLJS_Token CHTLJSLexer::parsePunctuation() {
    std::string value;
    value += currentChar();
    advance();
    
    CHTLJS_TokenType type = getPunctuationType(value);
    if (type != CHTLJS_TokenType::UNKNOWN) {
        return CHTLJS_Token(type, value, line_, column_);
    }
    
    return CHTLJS_Token(CHTLJS_TokenType::UNKNOWN, value, line_, column_);
}

bool CHTLJSLexer::isLetter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isLetter(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n';
}

bool CHTLJSLexer::isOperatorChar(char c) const {
    return c == '&' || c == '-' || c == '=' || c == '!' || c == '<' || c == '>' || 
           c == '&' || c == '|' || c == '+' || c == '*' || c == '/' || c == '%';
}

bool CHTLJSLexer::isPunctuationChar(char c) const {
    return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' ||
           c == ';' || c == ',' || c == '.' || c == ':' || c == '?';
}

CHTLJS_TokenType CHTLJSLexer::getKeywordType(const std::string& keyword) const {
    auto it = keywords_.find(keyword);
    if (it != keywords_.end()) {
        return it->second;
    }
    return CHTLJS_TokenType::UNKNOWN;
}

CHTLJS_TokenType CHTLJSLexer::getOperatorType(const std::string& op) const {
    auto it = operators_.find(op);
    if (it != operators_.end()) {
        return it->second;
    }
    return CHTLJS_TokenType::UNKNOWN;
}

CHTLJS_TokenType CHTLJSLexer::getPunctuationType(const std::string& punct) const {
    auto it = punctuation_.find(punct);
    if (it != punctuation_.end()) {
        return it->second;
    }
    return CHTLJS_TokenType::UNKNOWN;
}

} // namespace CHTLJS