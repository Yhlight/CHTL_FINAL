#include "CHTL/CHTLLexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& filename) 
    : isFile_(true), currentPos_(0), currentLine_(1), currentColumn_(1), 
      inComment_(false), inString_(false), stringDelimiter_('\0') {
    file_.open(filename);
    if (!file_.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    loadSource();
}

CHTLLexer::CHTLLexer(const std::string& source, bool isString) 
    : source_(source), isFile_(false), currentPos_(0), currentLine_(1), currentColumn_(1),
      inComment_(false), inString_(false), stringDelimiter_('\0') {
    if (!isString) {
        throw std::invalid_argument("字符串构造器需要isString=true");
    }
}

CHTLLexer::~CHTLLexer() {
    if (file_.is_open()) {
        file_.close();
    }
}

void CHTLLexer::loadSource() {
    if (isFile_) {
        std::stringstream buffer;
        buffer << file_.rdbuf();
        source_ = buffer.str();
    }
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (currentPos_ < source_.length()) {
        try {
            Token token = nextToken();
            if (token.getType() != TokenType::WHITESPACE && 
                token.getType() != TokenType::COMMENT) {
                tokens.push_back(token);
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            break;
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", currentLine_, currentColumn_);
    return tokens;
}

Token CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (currentPos_ >= source_.length()) {
        return Token(TokenType::EOF_TOKEN, "", currentLine_, currentColumn_);
    }
    
    char current = source_[currentPos_];
    
    // 处理注释
    if (current == '/' && currentPos_ + 1 < source_.length()) {
        char next = source_[currentPos_ + 1];
        if (next == '/') {
            return parseComment();
        } else if (next == '*') {
            return parseMultiLineComment();
        }
    }
    
    // 处理生成器注释 --
    if (current == '-' && currentPos_ + 1 < source_.length() && source_[currentPos_ + 1] == '-') {
        return parseGeneratorComment();
    }
    
    // 处理字符串
    if (current == '"' || current == '\'') {
        return parseString();
    }
    
    // 处理数字
    if (isDigit(current)) {
        return parseNumber();
    }
    
    // 处理标识符和关键字
    if (isAlpha(current) || current == '_') {
        return parseIdentifier();
    }
    
    // 处理方括号结构 [Template], [Custom], [Origin], [Import], [Namespace], [Configuration]
    if (current == '[') {
        return parseBracketStructure();
    }
    
    // 处理@符号结构 @Style, @Element, @Var, @Html, @JavaScript, @Chtl, @CJmod, @Config
    if (current == '@') {
        return parseAtStructure();
    }
    
    // 处理符号
    if (isSymbol(current)) {
        return parseSymbol();
    }
    
    // 处理字面量
    return parseLiteral();
}

void CHTLLexer::skipWhitespace() {
    while (currentPos_ < source_.length()) {
        char current = source_[currentPos_];
        if (isWhitespace(current)) {
            if (current == '\n') {
                currentLine_++;
                currentColumn_ = 1;
            } else {
                currentColumn_++;
            }
            currentPos_++;
        } else {
            break;
        }
    }
}

Token CHTLLexer::parseComment() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    // 跳过 //
    currentPos_ += 2;
    currentColumn_ += 2;
    
    while (currentPos_ < source_.length() && source_[currentPos_] != '\n') {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    return Token(TokenType::COMMENT, value, startLine, startColumn);
}

Token CHTLLexer::parseMultiLineComment() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    // 跳过 /*
    currentPos_ += 2;
    currentColumn_ += 2;
    
    while (currentPos_ < source_.length() - 1) {
        if (source_[currentPos_] == '*' && source_[currentPos_ + 1] == '/') {
            currentPos_ += 2;
            currentColumn_ += 2;
            break;
        }
        
        if (source_[currentPos_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        
        value += source_[currentPos_];
        currentPos_++;
    }
    
    return Token(TokenType::MULTI_COMMENT, value, startLine, startColumn);
}

Token CHTLLexer::parseGeneratorComment() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    // 跳过 --
    currentPos_ += 2;
    currentColumn_ += 2;
    
    while (currentPos_ < source_.length() && source_[currentPos_] != '\n') {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn);
}

Token CHTLLexer::parseString() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    char delimiter = source_[currentPos_];
    currentPos_++;
    currentColumn_++;
    
    while (currentPos_ < source_.length() && source_[currentPos_] != delimiter) {
        if (source_[currentPos_] == '\\' && currentPos_ + 1 < source_.length()) {
            currentPos_++;
            currentColumn_++;
            char escaped = source_[currentPos_];
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
            value += source_[currentPos_];
        }
        
        if (source_[currentPos_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        currentPos_++;
    }
    
    if (currentPos_ < source_.length()) {
        currentPos_++;
        currentColumn_++;
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn);
}

Token CHTLLexer::parseNumber() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    while (currentPos_ < source_.length() && 
           (isDigit(source_[currentPos_]) || source_[currentPos_] == '.')) {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn);
}

Token CHTLLexer::parseIdentifier() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    while (currentPos_ < source_.length() && 
           (isAlphaNumeric(source_[currentPos_]) || source_[currentPos_] == '_')) {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    // 检查是否为关键字
    TokenType type = getKeywordType(value);
    if (type != TokenType::IDENTIFIER) {
        return Token(type, value, startLine, startColumn);
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
}

Token CHTLLexer::parseSymbol() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    char current = source_[currentPos_];
    value += current;
    currentPos_++;
    currentColumn_++;
    
    // 处理双字符符号
    if (currentPos_ < source_.length()) {
        char next = source_[currentPos_];
        std::string twoChar = value + next;
        
        if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" || 
            twoChar == ">=" || twoChar == "&&" || twoChar == "||" ||
            twoChar == "++" || twoChar == "--" || twoChar == "**") {
            value += next;
            currentPos_++;
            currentColumn_++;
        }
    }
    
    // 映射到TokenType
    TokenType type = TokenType::IDENTIFIER; // 默认
    
    // 处理双字符运算符
    if (value.length() == 2) {
        if (value == "==") type = TokenType::DOUBLE_EQUALS;
        else if (value == "!=") type = TokenType::NOT_EQUALS;
        else if (value == "<=") type = TokenType::LESS_EQUALS;
        else if (value == ">=") type = TokenType::GREATER_EQUALS;
        else if (value == "&&") type = TokenType::AND;
        else if (value == "||") type = TokenType::OR;
        else if (value == "**") type = TokenType::POWER;
        else if (value == "++") type = TokenType::PLUS; // 简化处理
        else if (value == "--") type = TokenType::MINUS; // 简化处理
    } else {
        // 单字符运算符
        switch (current) {
            case '{': type = TokenType::LBRACE; break;
            case '}': type = TokenType::RBRACE; break;
            case '(': type = TokenType::LPAREN; break;
            case ')': type = TokenType::RPAREN; break;
            case '[': type = TokenType::LBRACKET; break;
            case ']': type = TokenType::RBRACKET; break;
            case ';': type = TokenType::SEMICOLON; break;
            case ':': type = TokenType::COLON; break;
            case '=': type = TokenType::EQUALS; break;
            case ',': type = TokenType::COMMA; break;
            case '.': type = TokenType::DOT; break;
            case '#': type = TokenType::HASH; break;
            case '/': type = TokenType::SLASH; break;
            case '\\': type = TokenType::BACKSLASH; break;
            case '*': type = TokenType::ASTERISK; break;
            case '+': type = TokenType::PLUS; break;
            case '-': type = TokenType::MINUS; break;
            case '%': type = TokenType::PERCENT; break;
            case '!': type = TokenType::EXCLAMATION; break;
            case '?': type = TokenType::QUESTION; break;
            case '&': type = TokenType::AMPERSAND; break;
            case '|': type = TokenType::PIPE; break;
            case '<': type = TokenType::LESS; break;
            case '>': type = TokenType::GREATER; break;
        }
    }
    
    return Token(type, value, startLine, startColumn);
}

Token CHTLLexer::parseLiteral() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    while (currentPos_ < source_.length() && 
           !isWhitespace(source_[currentPos_]) && 
           !isSymbol(source_[currentPos_])) {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    return Token(TokenType::LITERAL, value, startLine, startColumn);
}

Token CHTLLexer::parseBracketStructure() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    // 跳过 [
    currentPos_++;
    currentColumn_++;
    
    // 读取标识符
    while (currentPos_ < source_.length() && 
           (isAlphaNumeric(source_[currentPos_]) || source_[currentPos_] == '_')) {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    // 跳过 ]
    if (currentPos_ < source_.length() && source_[currentPos_] == ']') {
        currentPos_++;
        currentColumn_++;
    }
    
    // 映射到对应的TokenType
    TokenType type = TokenType::IDENTIFIER;
    if (value == "Template") {
        type = TokenType::TEMPLATE;
    } else if (value == "Custom") {
        type = TokenType::CUSTOM;
    } else if (value == "Origin") {
        type = TokenType::ORIGIN;
    } else if (value == "Import") {
        type = TokenType::IMPORT;
    } else if (value == "Namespace") {
        type = TokenType::NAMESPACE;
    } else if (value == "Configuration") {
        type = TokenType::CONFIGURATION;
    } else if (value == "Name") {
        type = TokenType::NAME;
    } else if (value == "OriginType") {
        type = TokenType::ORIGIN_TYPE;
    }
    
    return Token(type, value, startLine, startColumn);
}

Token CHTLLexer::parseAtStructure() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    std::string value;
    
    // 跳过 @
    currentPos_++;
    currentColumn_++;
    
    // 读取标识符
    while (currentPos_ < source_.length() && 
           (isAlphaNumeric(source_[currentPos_]) || source_[currentPos_] == '_')) {
        value += source_[currentPos_];
        currentPos_++;
        currentColumn_++;
    }
    
    // 映射到对应的TokenType
    TokenType type = TokenType::IDENTIFIER;
    if (value == "Style") {
        type = TokenType::TEMPLATE_STYLE; // 默认作为模板样式
    } else if (value == "Element") {
        type = TokenType::TEMPLATE_ELEMENT; // 默认作为模板元素
    } else if (value == "Var") {
        type = TokenType::TEMPLATE_VAR; // 默认作为模板变量
    } else if (value == "Html") {
        type = TokenType::ORIGIN_HTML;
    } else if (value == "JavaScript") {
        type = TokenType::ORIGIN_JAVASCRIPT;
    } else if (value == "Chtl") {
        type = TokenType::IMPORT_CHTL;
    } else if (value == "CJmod") {
        type = TokenType::IMPORT_CJMOD;
    } else if (value == "Config") {
        type = TokenType::CONFIG;
    }
    
    return Token(type, value, startLine, startColumn);
}

char CHTLLexer::peek() const {
    if (currentPos_ + 1 < source_.length()) {
        return source_[currentPos_ + 1];
    }
    return '\0';
}

char CHTLLexer::advance() {
    if (currentPos_ < source_.length()) {
        char current = source_[currentPos_];
        currentPos_++;
        if (current == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        return current;
    }
    return '\0';
}

bool CHTLLexer::match(char expected) {
    if (currentPos_ < source_.length() && source_[currentPos_] == expected) {
        advance();
        return true;
    }
    return false;
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
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLLexer::isSymbol(char c) const {
    return c == '{' || c == '}' || c == '(' || c == ')' || 
           c == '[' || c == ']' || c == ';' || c == ':' || 
           c == '=' || c == ',' || c == '.' || c == '/' || 
           c == '\\' || c == '*' || c == '+' || c == '-' || 
           c == '%' || c == '!' || c == '?' || c == '&' || 
           c == '|' || c == '<' || c == '>';
}

TokenType CHTLLexer::getKeywordType(const std::string& keyword) const {
    static const std::unordered_map<std::string, TokenType> keywords = {
        // 基础关键字
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"use", TokenType::USE},
        {"except", TokenType::EXCEPT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"inherit", TokenType::INHERIT_VALUE},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"at", TokenType::AT},
        {"top", TokenType::TOP_POS},
        {"bottom", TokenType::BOTTOM_POS},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        
        // HTML元素
        {"html", TokenType::HTML},
        {"head", TokenType::HEAD},
        {"title", TokenType::TITLE},
        {"body", TokenType::BODY},
        {"div", TokenType::DIV},
        {"span", TokenType::SPAN},
        {"p", TokenType::P},
        {"a", TokenType::A},
        {"img", TokenType::IMG},
        {"br", TokenType::BR},
        {"hr", TokenType::HR},
        {"h1", TokenType::H1},
        {"h2", TokenType::H2},
        {"h3", TokenType::H3},
        {"h4", TokenType::H4},
        {"h5", TokenType::H5},
        {"h6", TokenType::H6},
        {"ul", TokenType::UL},
        {"ol", TokenType::OL},
        {"li", TokenType::LI},
        {"table", TokenType::TABLE},
        {"tr", TokenType::TR},
        {"td", TokenType::TD},
        {"th", TokenType::TH},
        {"form", TokenType::FORM},
        {"input", TokenType::INPUT},
        {"button", TokenType::BUTTON},
        
        // 样式属性
        {"class", TokenType::CLASS},
        {"id", TokenType::ID},
        {"width", TokenType::WIDTH},
        {"height", TokenType::HEIGHT},
        {"color", TokenType::COLOR},
        {"background", TokenType::BACKGROUND},
        {"margin", TokenType::MARGIN},
        {"padding", TokenType::PADDING},
        {"border", TokenType::BORDER},
        {"font", TokenType::FONT},
        {"text-align", TokenType::TEXT_ALIGN},
        {"display", TokenType::DISPLAY},
        {"position", TokenType::POSITION},
        {"float", TokenType::FLOAT},
        {"clear", TokenType::CLEAR},
        {"overflow", TokenType::OVERFLOW},
        {"z-index", TokenType::Z_INDEX},
        {"opacity", TokenType::OPACITY},
        {"transform", TokenType::TRANSFORM},
        {"transition", TokenType::TRANSITION},
        {"animation", TokenType::ANIMATION},
        
        // 单位
        {"px", TokenType::PX},
        {"em", TokenType::EM},
        {"rem", TokenType::REM},
        {"vw", TokenType::VW},
        {"vh", TokenType::VH},
        
        // 颜色
        {"red", TokenType::RED},
        {"green", TokenType::GREEN},
        {"blue", TokenType::BLUE},
        {"white", TokenType::WHITE},
        {"black", TokenType::BLACK},
        {"yellow", TokenType::YELLOW},
        {"orange", TokenType::ORANGE},
        {"purple", TokenType::PURPLE},
        {"pink", TokenType::PINK},
        {"gray", TokenType::GRAY},
        {"transparent", TokenType::TRANSPARENT},
        
        // 位置
        {"left", TokenType::LEFT},
        {"right", TokenType::RIGHT},
        {"center", TokenType::CENTER},
        {"top", TokenType::TOP_POS},
        {"bottom", TokenType::BOTTOM_POS},
        {"middle", TokenType::MIDDLE},
        
        // 显示
        {"block", TokenType::BLOCK},
        {"inline", TokenType::INLINE},
        {"inline-block", TokenType::INLINE_BLOCK},
        {"flex", TokenType::FLEX},
        {"grid", TokenType::GRID},
        {"none", TokenType::NONE},
        
        // 其他
        {"auto", TokenType::AUTO},
        {"inherit", TokenType::INHERIT_VALUE},
        {"initial", TokenType::INITIAL},
        {"unset", TokenType::UNSET},
        {"important", TokenType::IMPORTANT},
        
        // CHTL JS相关
        {"vir", TokenType::VIR},
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE},
        {"router", TokenType::ROUTER},
        {"fileloader", TokenType::FILELOADER},
        {"iNeverAway", TokenType::INEVERAWAY},
        {"util", TokenType::UTIL},
        {"change", TokenType::CHANGE},
        {"then", TokenType::THEN},
    };
    
    auto it = keywords.find(keyword);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

bool CHTLLexer::isKeyword(const std::string& word) const {
    return getKeywordType(word) != TokenType::IDENTIFIER;
}

void CHTLLexer::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void CHTLLexer::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message, currentLine_, currentColumn_);
    } else {
        throw std::runtime_error("词法分析错误 [" + std::to_string(currentLine_) + 
                                ":" + std::to_string(currentColumn_) + "]: " + message);
    }
}

} // namespace CHTL