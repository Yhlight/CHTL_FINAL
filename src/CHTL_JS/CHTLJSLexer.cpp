#include "CHTL_JS/CHTLJSLexer.h"
#include <cctype>
#include <sstream>

namespace CHTL_JS {

// CHTLJSLexer 实现
CHTLJSLexer::CHTLJSLexer(std::istream& input)
    : input_(input), currentPos_(1, 1, 0), hasNextToken_(false) {
    // 预读第一个词法单元
    nextToken_ = scanToken();
    hasNextToken_ = true;
}

CHTLJSToken CHTLJSLexer::nextToken() {
    if (!hasNextToken_) {
        return CHTLJSToken(CHTLJSTokenType::EndOfFile, "", currentPos_);
    }
    
    CHTLJSToken token = nextToken_;
    nextToken_ = scanToken();
    hasNextToken_ = !nextToken_.isOperator() && nextToken_.type != CHTLJSTokenType::EndOfFile;
    
    return token;
}

CHTLJSToken CHTLJSLexer::peekToken() {
    return nextToken_;
}

void CHTLJSLexer::skipToken() {
    nextToken();
}

bool CHTLJSLexer::isEof() const {
    return nextToken_.type == CHTLJSTokenType::EndOfFile;
}

const Position& CHTLJSLexer::getPosition() const {
    return currentPos_;
}

const ErrorList& CHTLJSLexer::getErrors() const {
    return errors_;
}

void CHTLJSLexer::reset() {
    input_.clear();
    input_.seekg(0);
    currentPos_ = Position(1, 1, 0);
    hasNextToken_ = false;
    errors_.clear();
    nextToken_ = scanToken();
    hasNextToken_ = true;
}

CHTLJSToken CHTLJSLexer::scanToken() {
    skipWhitespace();
    
    if (input_.eof()) {
        return CHTLJSToken(CHTLJSTokenType::EndOfFile, "", currentPos_);
    }
    
    char c = getChar();
    
    // 单字符词法单元
    switch (c) {
        case '{': return CHTLJSToken(CHTLJSTokenType::LeftBrace, "{", currentPos_);
        case '}': return CHTLJSToken(CHTLJSTokenType::RightBrace, "}", currentPos_);
        case '[': return CHTLJSToken(CHTLJSTokenType::LeftBracket, "[", currentPos_);
        case ']': return CHTLJSToken(CHTLJSTokenType::RightBracket, "]", currentPos_);
        case '(': return CHTLJSToken(CHTLJSTokenType::LeftParen, "(", currentPos_);
        case ')': return CHTLJSToken(CHTLJSTokenType::RightParen, ")", currentPos_);
        case ';': return CHTLJSToken(CHTLJSTokenType::Semicolon, ";", currentPos_);
        case ':': return CHTLJSToken(CHTLJSTokenType::Colon, ":", currentPos_);
        case '=': return CHTLJSToken(CHTLJSTokenType::Equal, "=", currentPos_);
        case ',': return CHTLJSToken(CHTLJSTokenType::Comma, ",", currentPos_);
        case '.': return CHTLJSToken(CHTLJSTokenType::Dot, ".", currentPos_);
        case '?': return CHTLJSToken(CHTLJSTokenType::Question, "?", currentPos_);
        case '!': return CHTLJSToken(CHTLJSTokenType::Exclamation, "!", currentPos_);
        case '&': return CHTLJSToken(CHTLJSTokenType::Ampersand, "&", currentPos_);
        case '|': return CHTLJSToken(CHTLJSTokenType::Pipe, "|", currentPos_);
        case '~': return CHTLJSToken(CHTLJSTokenType::Tilde, "~", currentPos_);
        case '^': return CHTLJSToken(CHTLJSTokenType::Caret, "^", currentPos_);
        case '$': return CHTLJSToken(CHTLJSTokenType::Dollar, "$", currentPos_);
        case '%': return CHTLJSToken(CHTLJSTokenType::Percent, "%", currentPos_);
        case '+': return CHTLJSToken(CHTLJSTokenType::Plus, "+", currentPos_);
        case '-': return CHTLJSToken(CHTLJSTokenType::Minus, "-", currentPos_);
        case '*': return CHTLJSToken(CHTLJSTokenType::Star, "*", currentPos_);
        case '/': return CHTLJSToken(CHTLJSTokenType::Slash, "/", currentPos_);
        case '\\': return CHTLJSToken(CHTLJSTokenType::Backslash, "\\", currentPos_);
        case '<': return CHTLJSToken(CHTLJSTokenType::Less, "<", currentPos_);
        case '>': return CHTLJSToken(CHTLJSTokenType::Greater, ">", currentPos_);
        case '_': return CHTLJSToken(CHTLJSTokenType::Underscore, "_", currentPos_);
    }
    
    // 字符串
    if (isQuote(c)) {
        ungetChar();
        return scanString();
    }
    
    // 数字
    if (isDigit(c)) {
        ungetChar();
        return scanNumber();
    }
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        ungetChar();
        return scanIdentifier();
    }
    
    // 注释
    if (c == '/') {
        char next = peekChar();
        if (next == '/' || next == '*') {
            ungetChar();
            return scanComment();
        }
        return CHTLJSToken(CHTLJSTokenType::Slash, "/", currentPos_);
    }
    
    // 未知字符
    addError("Unknown character: " + String(1, c), currentPos_);
    return CHTLJSToken(CHTLJSTokenType::Error, String(1, c), currentPos_);
}

CHTLJSToken CHTLJSLexer::scanIdentifier() {
    String value;
    Position startPos = currentPos_;
    
    while (!input_.eof()) {
        char c = peekChar();
        if (isAlphaNumeric(c) || c == '_' || c == '-') {
            value += getChar();
        } else {
            break;
        }
    }
    
    // 检查是否为关键字
    if (isKeyword(value)) {
        return CHTLJSToken(CHTLJSTokenType::Keyword, value, startPos);
    }
    
    return CHTLJSToken(CHTLJSTokenType::Identifier, value, startPos);
}

CHTLJSToken CHTLJSLexer::scanString() {
    String value;
    Position startPos = currentPos_;
    char quote = getChar(); // 获取引号字符
    
    while (!input_.eof()) {
        char c = getChar();
        if (c == quote) {
            break;
        } else if (c == '\\' && !input_.eof()) {
            char next = getChar();
            switch (next) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += next; break;
            }
        } else {
            value += c;
        }
    }
    
    return CHTLJSToken(CHTLJSTokenType::String, value, startPos);
}

CHTLJSToken CHTLJSLexer::scanNumber() {
    String value;
    Position startPos = currentPos_;
    bool hasDot = false;
    
    while (!input_.eof()) {
        char c = peekChar();
        if (isDigit(c)) {
            value += getChar();
        } else if (c == '.' && !hasDot) {
            hasDot = true;
            value += getChar();
        } else {
            break;
        }
    }
    
    return CHTLJSToken(CHTLJSTokenType::Number, value, startPos);
}

CHTLJSToken CHTLJSLexer::scanComment() {
    String value;
    Position startPos = currentPos_;
    char c = getChar();
    
    if (c == '/') {
        // 单行注释 //
        char next = getChar();
        if (next == '/') {
            while (!input_.eof() && !isNewline(peekChar())) {
                value += getChar();
            }
            return CHTLJSToken(CHTLJSTokenType::Comment, value, startPos);
        } else {
            ungetChar();
            return CHTLJSToken(CHTLJSTokenType::Slash, "/", startPos);
        }
    } else if (c == '*') {
        // 多行注释 /* */
        while (!input_.eof()) {
            char next = getChar();
            if (next == '*' && !input_.eof() && peekChar() == '/') {
                getChar(); // 消费 '/'
                break;
            }
            value += next;
        }
        return CHTLJSToken(CHTLJSTokenType::Comment, value, startPos);
    }
    
    return CHTLJSToken(CHTLJSTokenType::Error, String(1, c), startPos);
}

CHTLJSToken CHTLJSLexer::scanOperator() {
    String value;
    Position startPos = currentPos_;
    char c = getChar();
    value += c;
    
    // 检查多字符操作符
    if (!input_.eof()) {
        char next = peekChar();
        String twoChar = value + next;
        if (isOperator(twoChar)) {
            getChar();
            return CHTLJSToken(CHTLJSTokenType::Keyword, twoChar, startPos);
        }
    }
    
    return CHTLJSToken(CHTLJSTokenType::Keyword, value, startPos);
}

CHTLJSToken CHTLJSLexer::scanPunctuation() {
    char c = getChar();
    return CHTLJSToken(CHTLJSTokenType::Error, String(1, c), currentPos_);
}

char CHTLJSLexer::getChar() {
    char c = input_.get();
    updatePosition(c);
    return c;
}

char CHTLJSLexer::peekChar() {
    char c = input_.peek();
    return c;
}

void CHTLJSLexer::ungetChar() {
    input_.unget();
    // 简单的位置回退，实际实现可能需要更复杂的位置管理
    if (currentPos_.column > 1) {
        currentPos_.column--;
    } else if (currentPos_.line > 1) {
        currentPos_.line--;
        currentPos_.column = 1; // 简化处理
    }
    currentPos_.offset--;
}

void CHTLJSLexer::skipWhitespace() {
    while (!input_.eof()) {
        char c = peekChar();
        if (isWhitespace(c)) {
            getChar();
        } else if (isNewline(c)) {
            skipNewline();
        } else {
            break;
        }
    }
}

void CHTLJSLexer::skipNewline() {
    char c = getChar();
    if (c == '\r' && !input_.eof() && peekChar() == '\n') {
        getChar(); // 消费 \n
    }
}

void CHTLJSLexer::updatePosition(char c) {
    currentPos_.offset++;
    if (c == '\n') {
        currentPos_.line++;
        currentPos_.column = 1;
    } else if (c == '\r') {
        // 处理 \r\n 的情况
        if (input_.eof() || peekChar() != '\n') {
            currentPos_.line++;
            currentPos_.column = 1;
        }
    } else {
        currentPos_.column++;
    }
}

void CHTLJSLexer::addError(const String& message, const Position& pos) {
    errors_.emplace_back(message, pos, "error");
}

bool CHTLJSLexer::isKeyword(const String& word) const {
    static const StringSet keywords = {
        // CHTL JS 特定关键字
        "vir", "listen", "delegate", "animate", "router", "fileloader", "util", "print", "iNeverAway",
        "printMylove", "iNeverAway", "util", "then", "change", "when", "begin", "end", "loop",
        "direction", "delay", "callback", "target", "duration", "easing", "at", "url", "mode",
        "width", "height", "scale", "Void", "function", "fun", "arrow", "=>", "->",
        
        // JavaScript 关键字
        "function", "var", "let", "const", "if", "else", "for", "while", "do", "switch", "case",
        "default", "break", "continue", "return", "try", "catch", "finally", "throw", "new",
        "this", "super", "class", "extends", "import", "export", "from", "as", "of", "in",
        "instanceof", "typeof", "void", "delete", "with", "debugger", "yield", "async", "await",
        "true", "false", "null", "undefined", "NaN", "Infinity", "console", "window", "document",
        "Object", "Array", "String", "Number", "Boolean", "Date", "RegExp", "Math", "JSON",
        "Promise", "Set", "Map", "WeakSet", "WeakMap", "Symbol", "Proxy", "Reflect",
        "addEventListener", "removeEventListener", "querySelector", "querySelectorAll",
        "getElementById", "getElementsByClassName", "getElementsByTagName", "createElement",
        "appendChild", "removeChild", "insertBefore", "replaceChild", "cloneNode",
        "setAttribute", "getAttribute", "removeAttribute", "hasAttribute", "classList",
        "style", "innerHTML", "textContent", "value", "checked", "selected", "disabled",
        "click", "mouseenter", "mouseleave", "mousemove", "mouseover", "mouseout",
        "keydown", "keyup", "keypress", "focus", "blur", "change", "submit", "load", "unload",
        "resize", "scroll", "animationstart", "animationend", "animationiteration",
        "transitionstart", "transitionend", "transitionrun", "transitioncancel"
    };
    
    return keywords.find(word) != keywords.end();
}

bool CHTLJSLexer::isOperator(const String& op) const {
    static const StringSet operators = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!",
        "&", "|", "^", "~", "<<", ">>", ">>>", "++", "--", "+=", "-=", "*=", "/=", "%=",
        "&=", "|=", "^=", "<<=", ">>=", ">>>=", "?", ":", "??", "?.", "?.[", "?(",
        "=>", "->", "::", "->>", "->", "=>", "<=>", "<=>", "<=>", "<=>", "<=>", "<=>",
        "===", "!==", "**", "**=", "||=", "&&=", "??=", "?.=", "?.[=", "?.("
    };
    
    return operators.find(op) != operators.end();
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t';
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

bool CHTLJSLexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLJSLexer::isEscape(char c) const {
    return c == '\\';
}

// CHTLJSToken 方法实现
bool CHTLJSToken::isOperator() const {
    static const StringSet operators = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!",
        "&", "|", "^", "~", "<<", ">>", ">>>", "++", "--", "+=", "-=", "*=", "/=", "%=",
        "&=", "|=", "^=", "<<=", ">>=", ">>>=", "?", ":", "??", "?.", "?.[", "?(",
        "=>", "->", "::", "->>", "->", "=>", "<=>", "<=>", "<=>", "<=>", "<=>", "<=>",
        "===", "!==", "**", "**=", "||=", "&&=", "??=", "?.=", "?.[=", "?.("
    };
    
    return operators.find(value) != operators.end();
}

bool CHTLJSToken::isPunctuation() const {
    static const StringSet punctuation = {
        "{", "}", "[", "]", "(", ")", ";", ":", ",", ".", "#", "@", "?", "!", "&", "|", "~", "^", "$", "%"
    };
    
    return punctuation.find(value) != punctuation.end();
}

bool CHTLJSToken::isKeyword(const String& keyword) const {
    return type == CHTLJSTokenType::Keyword && value == keyword;
}

bool CHTLJSToken::isOperator(const String& op) const {
    return type == CHTLJSTokenType::Keyword && value == op;
}

} // namespace CHTL_JS