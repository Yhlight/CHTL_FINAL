#include "CHTL/CHTLLexer.h"
#include <cctype>
#include <sstream>

namespace CHTL {

// CHTLLexer 实现
CHTLLexer::CHTLLexer(std::istream& input)
    : input_(input), currentPos_(1, 1, 0), hasNextToken_(false) {
    // 预读第一个词法单元
    nextToken_ = scanToken();
    hasNextToken_ = true;
}

Token CHTLLexer::nextToken() {
    if (!hasNextToken_) {
        return Token(TokenType::EndOfFile, "", currentPos_);
    }
    
    Token token = nextToken_;
    nextToken_ = scanToken();
    hasNextToken_ = !nextToken_.isOperator() && nextToken_.type != TokenType::EndOfFile;
    
    return token;
}

Token CHTLLexer::peekToken() {
    return nextToken_;
}

void CHTLLexer::skipToken() {
    nextToken();
}

bool CHTLLexer::isEof() const {
    return nextToken_.type == TokenType::EndOfFile;
}

const Position& CHTLLexer::getPosition() const {
    return currentPos_;
}

const ErrorList& CHTLLexer::getErrors() const {
    return errors_;
}

void CHTLLexer::reset() {
    input_.clear();
    input_.seekg(0);
    currentPos_ = Position(1, 1, 0);
    hasNextToken_ = false;
    errors_.clear();
    nextToken_ = scanToken();
    hasNextToken_ = true;
}

Token CHTLLexer::scanToken() {
    skipWhitespace();
    
    if (input_.eof()) {
        return Token(TokenType::EndOfFile, "", currentPos_);
    }
    
    char c = getChar();
    
    // 单字符词法单元
    switch (c) {
        case '{': return Token(TokenType::LeftBrace, "{", currentPos_);
        case '}': return Token(TokenType::RightBrace, "}", currentPos_);
        case '[': return Token(TokenType::LeftBracket, "[", currentPos_);
        case ']': return Token(TokenType::RightBracket, "]", currentPos_);
        case '(': return Token(TokenType::LeftParen, "(", currentPos_);
        case ')': return Token(TokenType::RightParen, ")", currentPos_);
        case ';': return Token(TokenType::Semicolon, ";", currentPos_);
        case ':': return Token(TokenType::Colon, ":", currentPos_);
        case '=': return Token(TokenType::Equal, "=", currentPos_);
        case ',': return Token(TokenType::Comma, ",", currentPos_);
        case '.': return Token(TokenType::Dot, ".", currentPos_);
        case '#': return Token(TokenType::Hash, "#", currentPos_);
        case '@': return Token(TokenType::At, "@", currentPos_);
        case '?': return Token(TokenType::Question, "?", currentPos_);
        case '!': return Token(TokenType::Exclamation, "!", currentPos_);
        case '&': return Token(TokenType::Ampersand, "&", currentPos_);
        case '|': return Token(TokenType::Pipe, "|", currentPos_);
        case '~': return Token(TokenType::Tilde, "~", currentPos_);
        case '^': return Token(TokenType::Caret, "^", currentPos_);
        case '$': return Token(TokenType::Dollar, "$", currentPos_);
        case '%': return Token(TokenType::Percent, "%", currentPos_);
        case '+': return Token(TokenType::Plus, "+", currentPos_);
        case '-': return Token(TokenType::Minus, "-", currentPos_);
        case '*': return Token(TokenType::Star, "*", currentPos_);
        case '/': return Token(TokenType::Slash, "/", currentPos_);
        case '\\': return Token(TokenType::Backslash, "\\", currentPos_);
        case '<': return Token(TokenType::Less, "<", currentPos_);
        case '>': return Token(TokenType::Greater, ">", currentPos_);
        case '_': return Token(TokenType::Underscore, "_", currentPos_);
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
        return Token(TokenType::Slash, "/", currentPos_);
    }
    
    // 减号开头的注释
    if (c == '-') {
        char next = peekChar();
        if (next == '-') {
            ungetChar();
            return scanComment();
        }
        return Token(TokenType::Minus, "-", currentPos_);
    }
    
    // 未知字符
    addError("Unknown character: " + String(1, c), currentPos_);
    return Token(TokenType::Error, String(1, c), currentPos_);
}

Token CHTLLexer::scanIdentifier() {
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
        return Token(TokenType::Keyword, value, startPos);
    }
    
    return Token(TokenType::Identifier, value, startPos);
}

Token CHTLLexer::scanString() {
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
    
    return Token(TokenType::String, value, startPos);
}

Token CHTLLexer::scanNumber() {
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
    
    return Token(TokenType::Number, value, startPos);
}

Token CHTLLexer::scanComment() {
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
            return Token(TokenType::Comment, value, startPos);
        } else {
            ungetChar();
            return Token(TokenType::Slash, "/", startPos);
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
        return Token(TokenType::Comment, value, startPos);
    } else if (c == '-') {
        // 生成器注释 --
        char next = getChar();
        if (next == '-') {
            while (!input_.eof() && !isNewline(peekChar())) {
                value += getChar();
            }
            return Token(TokenType::GeneratorComment, value, startPos);
        } else {
            ungetChar();
            return Token(TokenType::Minus, "-", startPos);
        }
    }
    
    return Token(TokenType::Error, String(1, c), startPos);
}

Token CHTLLexer::scanOperator() {
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
            return Token(TokenType::Keyword, twoChar, startPos);
        }
    }
    
    return Token(TokenType::Keyword, value, startPos);
}

Token CHTLLexer::scanPunctuation() {
    char c = getChar();
    return Token(TokenType::Error, String(1, c), currentPos_);
}

char CHTLLexer::getChar() {
    char c = input_.get();
    updatePosition(c);
    return c;
}

char CHTLLexer::peekChar() {
    char c = input_.peek();
    return c;
}

void CHTLLexer::ungetChar() {
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

void CHTLLexer::skipWhitespace() {
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

void CHTLLexer::skipNewline() {
    char c = getChar();
    if (c == '\r' && !input_.eof() && peekChar() == '\n') {
        getChar(); // 消费 \n
    }
}

void CHTLLexer::updatePosition(char c) {
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

void CHTLLexer::addError(const String& message, const Position& pos) {
    errors_.emplace_back(message, pos, "error");
}

bool CHTLLexer::isKeyword(const String& word) const {
    static const StringSet keywords = {
        "text", "style", "script", "html", "head", "body", "div", "span", "p", "a", "img",
        "ul", "ol", "li", "table", "tr", "td", "th", "form", "input", "button", "select",
        "option", "textarea", "label", "h1", "h2", "h3", "h4", "h5", "h6", "br", "hr",
        "meta", "title", "link", "script", "style", "template", "custom", "origin",
        "import", "config", "namespace", "except", "delete", "insert", "use", "inherit",
        "from", "as", "at", "top", "bottom", "after", "before", "replace", "and", "or",
        "not", "true", "false", "null", "undefined", "if", "else", "for", "while", "do",
        "switch", "case", "default", "break", "continue", "return", "function", "var",
        "let", "const", "class", "interface", "enum", "type", "namespace", "module",
        "export", "import", "public", "private", "protected", "static", "abstract",
        "virtual", "override", "final", "async", "await", "yield", "generator"
    };
    
    return keywords.find(word) != keywords.end();
}

bool CHTLLexer::isOperator(const String& op) const {
    static const StringSet operators = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!",
        "&", "|", "^", "~", "<<", ">>", ">>>", "++", "--", "+=", "-=", "*=", "/=", "%=",
        "&=", "|=", "^=", "<<=", ">>=", ">>>=", "?", ":", "??", "?.", "?.[", "?(",
        "=>", "->", "::", "->>", "->", "=>", "<=>", "<=>", "<=>", "<=>", "<=>", "<=>"
    };
    
    return operators.find(op) != operators.end();
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

bool CHTLLexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isEscape(char c) const {
    return c == '\\';
}

// Token 方法实现
bool Token::isOperator() const {
    static const StringSet operators = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!",
        "&", "|", "^", "~", "<<", ">>", ">>>", "++", "--", "+=", "-=", "*=", "/=", "%=",
        "&=", "|=", "^=", "<<=", ">>=", ">>>=", "?", ":", "??", "?.", "?.[", "?(",
        "=>", "->", "::", "->>", "->", "=>", "<=>", "<=>", "<=>", "<=>", "<=>", "<=>"
    };
    
    return operators.find(value) != operators.end();
}

bool Token::isPunctuation() const {
    static const StringSet punctuation = {
        "{", "}", "[", "]", "(", ")", ";", ":", ",", ".", "#", "@", "?", "!", "&", "|", "~", "^", "$", "%"
    };
    
    return punctuation.find(value) != punctuation.end();
}

bool Token::isKeyword(const String& keyword) const {
    return type == TokenType::Keyword && value == keyword;
}

bool Token::isOperator(const String& op) const {
    return type == TokenType::Keyword && value == op;
}

} // namespace CHTL