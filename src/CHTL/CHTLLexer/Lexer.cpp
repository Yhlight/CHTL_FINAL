#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

Lexer::Lexer() : m_position(0), m_line(1), m_column(1), m_debugMode(false) {
    initializeKeywords();
}

void Lexer::initializeKeywords() {
    // CHTL关键字
    m_keywords = {
        "text", "style", "script", "template", "custom", "origin", "import",
        "namespace", "configuration", "info", "export", "delete", "insert",
        "inherit", "except", "use", "from", "as", "after", "before", "replace",
        "at", "top", "bottom", "vir", "listen", "animate", "router", "fileloader",
        "delegate", "iNeverAway", "util", "change", "then", "printMylove"
    };
    
    // 操作符
    m_operators = {
        "?", "&&", "||", ">", "<", ">=", "<=", "==", "!=", "+", "-", "*", "/", "%"
    };
    
    // 分隔符
    m_delimiters = {
        "{", "}", "(", ")", "[", "]", ";", ",", ".", ":", "="
    };
}

std::unique_ptr<TokenStream> Lexer::tokenize(const std::string& source) {
    m_source = source;
    m_position = 0;
    m_line = 1;
    m_column = 1;
    clearErrors();
    
    auto tokens = std::make_unique<TokenStream>();
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        char c = peek();
        
        if (isAlpha(c) || c == '_') {
            auto token = scanIdentifier();
            if (token) {
                tokens->addToken(std::move(token));
            }
        } else if (c == '"' || c == '\'') {
            auto token = scanString();
            if (token) {
                tokens->addToken(std::move(token));
            }
        } else if (isDigit(c)) {
            auto token = scanNumber();
            if (token) {
                tokens->addToken(std::move(token));
            }
        } else if (c == '/' && peek(1) == '/') {
            if (!skipComment()) {
                addError("无法解析注释");
                break;
            }
        } else if (c == '/' && peek(1) == '*') {
            if (!skipComment()) {
                addError("无法解析多行注释");
                break;
            }
        } else if (c == '-' && peek(1) == '-') {
            if (!skipComment()) {
                addError("无法解析生成器注释");
                break;
            }
        } else if (isOperatorChar(c)) {
            auto token = scanOperator();
            if (token) {
                tokens->addToken(std::move(token));
            }
        } else if (isDelimiterChar(c)) {
            auto token = scanDelimiter();
            if (token) {
                tokens->addToken(std::move(token));
            }
        } else if (isSpecialChar(c)) {
            auto token = scanSpecial();
            if (token) {
                tokens->addToken(std::move(token));
            }
        } else {
            // 尝试识别无修饰字面量
            auto token = scanLiteral();
            if (token) {
                tokens->addToken(std::move(token));
            } else {
                addError("无法识别的字符: " + std::string(1, c));
                consume();
            }
        }
    }
    
    // 添加文件结束令牌
    tokens->addToken(createToken(TokenType::END_OF_FILE, ""));
    
    return tokens;
}

std::unique_ptr<TokenStream> Lexer::tokenizeFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        addError("无法打开文件: " + filename);
        return nullptr;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return tokenize(buffer.str());
}

void Lexer::setDebugMode(bool debug) {
    m_debugMode = debug;
}

const std::vector<std::string>& Lexer::getErrors() const {
    return m_errors;
}

void Lexer::clearErrors() {
    m_errors.clear();
}

char Lexer::peek() const {
    return peek(0);
}

char Lexer::peek(size_t offset) const {
    size_t pos = m_position + offset;
    return pos >= m_source.length() ? '\0' : m_source[pos];
}

char Lexer::consume() {
    if (isAtEnd()) return '\0';
    
    char c = m_source[m_position++];
    if (c == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
    return c;
}

void Lexer::consume(size_t count) {
    for (size_t i = 0; i < count && !isAtEnd(); ++i) {
        consume();
    }
}

bool Lexer::isAtEnd() const {
    return m_position >= m_source.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        consume();
    }
}

bool Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        skipSingleLineComment();
        return true;
    } else if (peek() == '/' && peek(1) == '*') {
        skipMultiLineComment();
        return true;
    } else if (peek() == '-' && peek(1) == '-') {
        skipGeneratorComment();
        return true;
    }
    return false;
}

void Lexer::skipSingleLineComment() {
    consume(2); // 消费 "//"
    while (!isAtEnd() && peek() != '\n') {
        consume();
    }
}

void Lexer::skipMultiLineComment() {
    consume(2); // 消费 "/*"
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            consume(2); // 消费 "*/"
            break;
        }
        consume();
    }
}

void Lexer::skipGeneratorComment() {
    consume(2); // 消费 "--"
    while (!isAtEnd() && peek() != '\n') {
        consume();
    }
}

std::unique_ptr<Token> Lexer::scanIdentifier() {
    size_t start = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        consume();
    }
    
    std::string value = m_source.substr(start, m_position - start);
    TokenType type = TokenType::IDENTIFIER;
    
    if (isKeyword(value)) {
        type = getKeywordType(value);
    }
    
    return createToken(type, value);
}

std::unique_ptr<Token> Lexer::scanString() {
    char quote = consume(); // 消费开始引号
    size_t start = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\') {
            consume(); // 消费反斜杠
            if (!isAtEnd()) {
                consume(); // 消费转义字符
            }
        } else {
            consume();
        }
    }
    
    if (isAtEnd()) {
        addError("未闭合的字符串");
        return nullptr;
    }
    
    std::string value = m_source.substr(start, m_position - start);
    consume(); // 消费结束引号
    
    return createToken(TokenType::STRING, value);
}

std::unique_ptr<Token> Lexer::scanNumber() {
    size_t start = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (!isAtEnd() && isDigit(peek())) {
        consume();
    }
    
    // 检查小数点
    if (peek() == '.' && isDigit(peek(1))) {
        consume(); // 消费小数点
        while (!isAtEnd() && isDigit(peek())) {
            consume();
        }
    }
    
    std::string value = m_source.substr(start, m_position - start);
    return createToken(TokenType::NUMBER, value);
}

std::unique_ptr<Token> Lexer::scanLiteral() {
    size_t start = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (!isAtEnd() && !isWhitespace(peek()) && !isDelimiterChar(peek()) && 
           !isOperatorChar(peek()) && peek() != '{' && peek() != '}') {
        consume();
    }
    
    if (m_position == start) {
        return nullptr;
    }
    
    std::string value = m_source.substr(start, m_position - start);
    return createToken(TokenType::LITERAL, value);
}

std::unique_ptr<Token> Lexer::scanOperator() {
    size_t start = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    // 检查双字符操作符
    if (m_position + 1 < m_source.length()) {
        std::string twoChar = m_source.substr(m_position, 2);
        if (isOperator(twoChar)) {
            consume(2);
            return createToken(getOperatorType(twoChar), twoChar);
        }
    }
    
    // 单字符操作符
    char c = consume();
    std::string op(1, c);
    return createToken(getOperatorType(op), op);
}

std::unique_ptr<Token> Lexer::scanDelimiter() {
    char c = consume();
    std::string delim(1, c);
    return createToken(getDelimiterType(delim), delim);
}

std::unique_ptr<Token> Lexer::scanSpecial() {
    char c = consume();
    std::string special(1, c);
    
    TokenType type;
    switch (c) {
        case '&': type = TokenType::AMPERSAND; break;
        case '#': type = TokenType::HASH; break;
        case '$': type = TokenType::DOLLAR; break;
        case '%': type = TokenType::PERCENT; break;
        default: type = TokenType::ERROR; break;
    }
    
    return createToken(type, special);
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
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Lexer::isOperatorChar(char c) const {
    return c == '?' || c == '&' || c == '|' || c == '>' || c == '<' || 
           c == '=' || c == '!' || c == '+' || c == '-' || c == '*' || 
           c == '/' || c == '%';
}

bool Lexer::isDelimiterChar(char c) const {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || 
           c == ']' || c == ';' || c == ',' || c == '.' || c == ':' || c == '=';
}

bool Lexer::isSpecialChar(char c) const {
    return c == '&' || c == '#' || c == '$' || c == '%';
}

bool Lexer::isKeyword(const std::string& str) const {
    return m_keywords.find(str) != m_keywords.end();
}

bool Lexer::isOperator(const std::string& str) const {
    return m_operators.find(str) != m_operators.end();
}

bool Lexer::isDelimiter(const std::string& str) const {
    return m_delimiters.find(str) != m_delimiters.end();
}

TokenType Lexer::getKeywordType(const std::string& keyword) const {
    if (keyword == "text") return TokenType::TEXT;
    if (keyword == "style") return TokenType::STYLE;
    if (keyword == "script") return TokenType::SCRIPT;
    if (keyword == "template") return TokenType::TEMPLATE;
    if (keyword == "custom") return TokenType::CUSTOM;
    if (keyword == "origin") return TokenType::ORIGIN;
    if (keyword == "import") return TokenType::IMPORT;
    if (keyword == "namespace") return TokenType::NAMESPACE;
    if (keyword == "configuration") return TokenType::CONFIGURATION;
    if (keyword == "info") return TokenType::INFO;
    if (keyword == "export") return TokenType::EXPORT;
    if (keyword == "delete") return TokenType::DELETE;
    if (keyword == "insert") return TokenType::INSERT;
    if (keyword == "inherit") return TokenType::INHERIT;
    if (keyword == "except") return TokenType::EXCEPT;
    if (keyword == "use") return TokenType::USE;
    if (keyword == "from") return TokenType::FROM;
    if (keyword == "as") return TokenType::AS;
    if (keyword == "after") return TokenType::AFTER;
    if (keyword == "before") return TokenType::BEFORE;
    if (keyword == "replace") return TokenType::REPLACE;
    if (keyword == "at") return TokenType::AT_TOP; // 简化处理
    if (keyword == "top") return TokenType::AT_TOP;
    if (keyword == "bottom") return TokenType::AT_BOTTOM;
    if (keyword == "vir") return TokenType::VIR;
    if (keyword == "listen") return TokenType::LISTEN;
    if (keyword == "animate") return TokenType::ANIMATE;
    if (keyword == "router") return TokenType::ROUTER;
    if (keyword == "fileloader") return TokenType::FILELOADER;
    if (keyword == "delegate") return TokenType::DELEGATE;
    if (keyword == "iNeverAway") return TokenType::INEVERAWAY;
    if (keyword == "util") return TokenType::UTIL;
    if (keyword == "change") return TokenType::CHANGE;
    if (keyword == "then") return TokenType::THEN;
    if (keyword == "printMylove") return TokenType::PRINTMYLOVE;
    
    return TokenType::IDENTIFIER;
}

TokenType Lexer::getOperatorType(const std::string& op) const {
    if (op == "?") return TokenType::QUESTION;
    if (op == "&&") return TokenType::LOGICAL_AND;
    if (op == "||") return TokenType::LOGICAL_OR;
    if (op == ">") return TokenType::GREATER;
    if (op == "<") return TokenType::LESS;
    if (op == ">=") return TokenType::GREATER_EQUAL;
    if (op == "<=") return TokenType::LESS_EQUAL;
    if (op == "==") return TokenType::EQUAL;
    if (op == "!=") return TokenType::NOT_EQUAL;
    if (op == "+") return TokenType::PLUS;
    if (op == "-") return TokenType::MINUS;
    if (op == "*") return TokenType::MULTIPLY;
    if (op == "/") return TokenType::DIVIDE;
    if (op == "%") return TokenType::MODULO;
    
    return TokenType::ERROR;
}

TokenType Lexer::getDelimiterType(const std::string& delim) const {
    if (delim == "{") return TokenType::LEFT_BRACE;
    if (delim == "}") return TokenType::RIGHT_BRACE;
    if (delim == "(") return TokenType::LEFT_PAREN;
    if (delim == ")") return TokenType::RIGHT_PAREN;
    if (delim == "[") return TokenType::LEFT_BRACKET;
    if (delim == "]") return TokenType::RIGHT_BRACKET;
    if (delim == ";") return TokenType::SEMICOLON;
    if (delim == ",") return TokenType::COMMA;
    if (delim == ".") return TokenType::DOT;
    if (delim == ":") return TokenType::COLON;
    if (delim == "=") return TokenType::EQUALS;
    
    return TokenType::ERROR;
}

void Lexer::addError(const std::string& message) {
    std::string error = "行 " + std::to_string(m_line) + ", 列 " + 
                       std::to_string(m_column) + ": " + message;
    m_errors.push_back(error);
}

std::unique_ptr<Token> Lexer::createToken(TokenType type, const std::string& value) {
    return std::make_unique<Token>(type, value, m_line, m_column);
}

} // namespace CHTL