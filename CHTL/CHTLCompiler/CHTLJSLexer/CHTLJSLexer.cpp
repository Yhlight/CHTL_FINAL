#include "CHTLJSLexer.h"
#include <cctype>
#include <sstream>
#include <set>

namespace CHTL {

CHTLJSLexer::CHTLJSLexer(const std::string& source) 
    : source(source), position(0), line(1), column(1) {
}

CHTLJSLexer::~CHTLJSLexer() {
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    
    while (!isAtEnd()) {
        CHTLJSToken token = nextToken();
        tokens.push_back(token);
        
        if (token.type == CHTLJSTokenType::Error) {
            break;
        }
    }
    
    tokens.push_back(CHTLJSToken(CHTLJSTokenType::EndOfFile, "", line, column));
    return tokens;
}

CHTLJSToken CHTLJSLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return CHTLJSToken(CHTLJSTokenType::EndOfFile, "", line, column);
    }
    
    char c = currentChar();
    
    // Check for comments
    if (isCommentStart(c)) {
        return parseComment();
    }
    
    // Check for strings
    if (isStringStart(c)) {
        return parseString();
    }
    
    // Check for numbers
    if (isNumberStart(c)) {
        return parseNumber();
    }
    
    // Check for identifiers
    if (isIdentifierStart(c)) {
        return parseIdentifier();
    }
    
    // Check for operators
    if (isOperatorStart(c)) {
        return parseOperator();
    }
    
    // Check for punctuation
    if (isPunctuationStart(c)) {
        return parsePunctuation();
    }
    
    // Check for CHTL JS specific tokens
    if (isCHTLJSStart(c)) {
        return parseCHTLJSFunction();
    }
    
    // Unknown character
    return createErrorToken("Unexpected character: " + std::string(1, c));
}

CHTLJSToken CHTLJSLexer::peekToken() {
    size_t saved_position = position;
    size_t saved_line = line;
    size_t saved_column = column;
    
    CHTLJSToken token = nextToken();
    
    position = saved_position;
    line = saved_line;
    column = saved_column;
    
    return token;
}

bool CHTLJSLexer::isAtEnd() const {
    return position >= source.length();
}

void CHTLJSLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
}

char CHTLJSLexer::currentChar() const {
    if (isAtEnd()) {
        return '\0';
    }
    return source[position];
}

char CHTLJSLexer::peekChar() const {
    return peekChar(1);
}

char CHTLJSLexer::peekChar(size_t offset) const {
    if (position + offset >= source.length()) {
        return '\0';
    }
    return source[position + offset];
}

void CHTLJSLexer::advance() {
    if (!isAtEnd()) {
        if (isNewline(currentChar())) {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLJSLexer::advance(size_t count) {
    for (size_t i = 0; i < count; i++) {
        advance();
    }
}

bool CHTLJSLexer::match(char expected) {
    if (isAtEnd() || currentChar() != expected) {
        return false;
    }
    advance();
    return true;
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return std::isspace(c);
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

CHTLJSToken CHTLJSLexer::parseIdentifier() {
    std::string value = parseIdentifierName();
    
    // Check if it's a keyword
    if (isKeyword(value)) {
        return CHTLJSToken(getTokenType(value), value, line, column - value.length());
    }
    
    // Check if it's a CHTL JS keyword
    if (isCHTLJSKeyword(value)) {
        return CHTLJSToken(getTokenType(value), value, line, column - value.length());
    }
    
    return CHTLJSToken(CHTLJSTokenType::Identifier, value, line, column - value.length());
}

CHTLJSToken CHTLJSLexer::parseString() {
    char quote = currentChar();
    advance(); // Skip opening quote
    
    std::string value;
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance(); // Skip backslash
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
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (isAtEnd()) {
        return createErrorToken("Unterminated string");
    }
    
    advance(); // Skip closing quote
    return CHTLJSToken(CHTLJSTokenType::String, value, line, column - value.length() - 2);
}

CHTLJSToken CHTLJSLexer::parseNumber() {
    std::string value;
    
    if (currentChar() == '0' && peekChar() == 'x') {
        // Hex number
        advance(2);
        value = "0x" + parseHex();
    } else if (currentChar() == '0' && peekChar() == 'b') {
        // Binary number
        advance(2);
        value = "0b" + parseBinary();
    } else if (currentChar() == '0' && peekChar() == 'o') {
        // Octal number
        advance(2);
        value = "0o" + parseOctal();
    } else {
        // Decimal number
        if (currentChar() == '.') {
            value = parseFloat();
        } else {
            value = parseInteger();
            if (currentChar() == '.') {
                advance();
                value += "." + parseInteger();
            }
        }
        
        // Scientific notation
        if (currentChar() == 'e' || currentChar() == 'E') {
            advance();
            value += currentChar();
            if (currentChar() == '+' || currentChar() == '-') {
                advance();
                value += currentChar();
            }
            value += parseInteger();
        }
    }
    
    return CHTLJSToken(CHTLJSTokenType::Number, value, line, column - value.length());
}

CHTLJSToken CHTLJSLexer::parseComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        skipSingleLineComment();
        return nextToken(); // Return next token after skipping comment
    } else if (currentChar() == '/' && peekChar() == '*') {
        skipMultiLineComment();
        return nextToken(); // Return next token after skipping comment
    }
    
    return createErrorToken("Invalid comment");
}

CHTLJSToken CHTLJSLexer::parseOperator() {
    char c = currentChar();
    size_t start_column = column;
    
    switch (c) {
        case '+':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::PlusAssign, "+=", line, start_column);
            if (match('+')) return CHTLJSToken(CHTLJSTokenType::Plus, "++", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::Plus, "+", line, start_column);
            
        case '-':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::MinusAssign, "-=", line, start_column);
            if (match('-')) return CHTLJSToken(CHTLJSTokenType::Minus, "--", line, start_column);
            if (match('>')) return CHTLJSToken(CHTLJSTokenType::Arrow, "->", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::Minus, "-", line, start_column);
            
        case '*':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::MultiplyAssign, "*=", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::Multiply, "*", line, start_column);
            
        case '/':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::DivideAssign, "/=", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::Divide, "/", line, start_column);
            
        case '%':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::ModuloAssign, "%=", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::Modulo, "%", line, start_column);
            
        case '=':
            advance();
            if (match('=')) {
                if (match('=')) return CHTLJSToken(CHTLJSTokenType::Equal, "===", line, start_column);
                return CHTLJSToken(CHTLJSTokenType::Equal, "==", line, start_column);
            }
            return CHTLJSToken(CHTLJSTokenType::Assign, "=", line, start_column);
            
        case '!':
            advance();
            if (match('=')) {
                if (match('=')) return CHTLJSToken(CHTLJSTokenType::NotEqual, "!==", line, start_column);
                return CHTLJSToken(CHTLJSTokenType::NotEqual, "!=", line, start_column);
            }
            return CHTLJSToken(CHTLJSTokenType::Not, "!", line, start_column);
            
        case '<':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::LessEqual, "<=", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::LessThan, "<", line, start_column);
            
        case '>':
            advance();
            if (match('=')) return CHTLJSToken(CHTLJSTokenType::GreaterEqual, ">=", line, start_column);
            return CHTLJSToken(CHTLJSTokenType::GreaterThan, ">", line, start_column);
            
        case '&':
            advance();
            if (match('&')) return CHTLJSToken(CHTLJSTokenType::And, "&&", line, start_column);
            return createErrorToken("Unexpected character: &");
            
        case '|':
            advance();
            if (match('|')) return CHTLJSToken(CHTLJSTokenType::Or, "||", line, start_column);
            return createErrorToken("Unexpected character: |");
            
        default:
            return createErrorToken("Unexpected character: " + std::string(1, c));
    }
}

CHTLJSToken CHTLJSLexer::parsePunctuation() {
    char c = currentChar();
    size_t start_column = column;
    advance();
    
    switch (c) {
        case '(': return CHTLJSToken(CHTLJSTokenType::LeftParen, "(", line, start_column);
        case ')': return CHTLJSToken(CHTLJSTokenType::RightParen, ")", line, start_column);
        case '[': return CHTLJSToken(CHTLJSTokenType::LeftBracket, "[", line, start_column);
        case ']': return CHTLJSToken(CHTLJSTokenType::RightBracket, "]", line, start_column);
        case '{': return CHTLJSToken(CHTLJSTokenType::LeftBrace, "{", line, start_column);
        case '}': return CHTLJSToken(CHTLJSTokenType::RightBrace, "}", line, start_column);
        case ',': return CHTLJSToken(CHTLJSTokenType::Comma, ",", line, start_column);
        case ';': return CHTLJSToken(CHTLJSTokenType::Semicolon, ";", line, start_column);
        case ':': return CHTLJSToken(CHTLJSTokenType::Colon, ":", line, start_column);
        case '.': return CHTLJSToken(CHTLJSTokenType::Dot, ".", line, start_column);
        case '?': return CHTLJSToken(CHTLJSTokenType::QuestionMark, "?", line, start_column);
        default: return createErrorToken("Unexpected character: " + std::string(1, c));
    }
}

CHTLJSToken CHTLJSLexer::parseKeyword() {
    std::string value = parseIdentifierName();
    return CHTLJSToken(getTokenType(value), value, line, column - value.length());
}

CHTLJSToken CHTLJSLexer::parseCHTLJSKeyword() {
    std::string value = parseIdentifierName();
    return CHTLJSToken(getTokenType(value), value, line, column - value.length());
}

std::string CHTLJSLexer::parseStringLiteral() {
    return parseString().value;
}

std::string CHTLJSLexer::parseTemplateLiteral() {
    // Template literals start with backtick
    advance(); // Skip opening backtick
    
    std::string value;
    while (!isAtEnd() && currentChar() != '`') {
        if (currentChar() == '\\') {
            advance(); // Skip backslash
            if (!isAtEnd()) {
                value += currentChar();
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (!isAtEnd()) {
        advance(); // Skip closing backtick
    }
    
    return value;
}

std::string CHTLJSLexer::parseInteger() {
    std::string value;
    while (!isAtEnd() && isDigit(currentChar())) {
        value += currentChar();
        advance();
    }
    return value;
}

std::string CHTLJSLexer::parseFloat() {
    std::string value = "0.";
    advance(); // Skip the dot
    while (!isAtEnd() && isDigit(currentChar())) {
        value += currentChar();
        advance();
    }
    return value;
}

std::string CHTLJSLexer::parseHex() {
    std::string value;
    while (!isAtEnd() && (isDigit(currentChar()) || (currentChar() >= 'a' && currentChar() <= 'f') || (currentChar() >= 'A' && currentChar() <= 'F'))) {
        value += currentChar();
        advance();
    }
    return value;
}

std::string CHTLJSLexer::parseBinary() {
    std::string value;
    while (!isAtEnd() && (currentChar() == '0' || currentChar() == '1')) {
        value += currentChar();
        advance();
    }
    return value;
}

std::string CHTLJSLexer::parseOctal() {
    std::string value;
    while (!isAtEnd() && currentChar() >= '0' && currentChar() <= '7') {
        value += currentChar();
        advance();
    }
    return value;
}

std::string CHTLJSLexer::parseIdentifierName() {
    std::string value;
    while (!isAtEnd() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    return value;
}

bool CHTLJSLexer::isKeyword(const std::string& word) const {
    static const std::set<std::string> keywords = {
        "if", "else", "for", "while", "do", "switch", "case", "default",
        "break", "continue", "return", "function", "var", "let", "const",
        "class", "extends", "import", "export", "from", "as", "new", "this",
        "super", "static", "async", "await", "try", "catch", "finally",
        "throw", "typeof", "instanceof", "in", "of", "with", "delete", "void",
        "true", "false", "null", "undefined"
    };
    return keywords.find(word) != keywords.end();
}

bool CHTLJSLexer::isCHTLJSKeyword(const std::string& word) const {
    static const std::set<std::string> chtl_js_keywords = {
        "listen", "delegate", "animate", "vir", "router", "fileloader",
        "script", "selector", "listener"
    };
    return chtl_js_keywords.find(word) != chtl_js_keywords.end();
}

CHTLJSToken CHTLJSLexer::createErrorToken(const std::string& message) {
    return CHTLJSToken(CHTLJSTokenType::Error, message, line, column);
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLJSLexer::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        skipSingleLineComment();
    } else if (currentChar() == '/' && peekChar() == '*') {
        skipMultiLineComment();
    }
}

void CHTLJSLexer::skipSingleLineComment() {
    while (!isAtEnd() && !isNewline(currentChar())) {
        advance();
    }
}

void CHTLJSLexer::skipMultiLineComment() {
    advance(2); // Skip /*
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(2); // Skip */
            break;
        }
        advance();
    }
}

CHTLJSToken CHTLJSLexer::parseCHTLJSFunction() {
    // This would parse CHTL JS specific function syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseVirtualObject() {
    // This would parse virtual object syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseRouter() {
    // This would parse router syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseAnimation() {
    // This would parse animation syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseEventDelegation() {
    // This would parse event delegation syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseEnhancedSelector() {
    // This would parse enhanced selector syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseEnhancedListener() {
    // This would parse enhanced listener syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseFileLoader() {
    // This would parse file loader syntax
    return parseIdentifier();
}

CHTLJSToken CHTLJSLexer::parseLocalScript() {
    // This would parse local script syntax
    return parseIdentifier();
}

std::string CHTLJSLexer::getTokenValue(CHTLJSTokenType type) const {
    // This would return the string representation of a token type
    return "";
}

CHTLJSTokenType CHTLJSLexer::getTokenType(const std::string& value) const {
    // Map string values to token types
    if (value == "if") return CHTLJSTokenType::If;
    if (value == "else") return CHTLJSTokenType::Else;
    if (value == "for") return CHTLJSTokenType::For;
    if (value == "while") return CHTLJSTokenType::While;
    if (value == "do") return CHTLJSTokenType::Do;
    if (value == "switch") return CHTLJSTokenType::Switch;
    if (value == "case") return CHTLJSTokenType::Case;
    if (value == "default") return CHTLJSTokenType::Default;
    if (value == "break") return CHTLJSTokenType::Break;
    if (value == "continue") return CHTLJSTokenType::Continue;
    if (value == "return") return CHTLJSTokenType::Return;
    if (value == "function") return CHTLJSTokenType::Function;
    if (value == "var") return CHTLJSTokenType::Var;
    if (value == "let") return CHTLJSTokenType::Let;
    if (value == "const") return CHTLJSTokenType::Const;
    if (value == "class") return CHTLJSTokenType::Class;
    if (value == "extends") return CHTLJSTokenType::Extends;
    if (value == "import") return CHTLJSTokenType::Import;
    if (value == "export") return CHTLJSTokenType::Export;
    if (value == "from") return CHTLJSTokenType::From;
    if (value == "as") return CHTLJSTokenType::As;
    if (value == "new") return CHTLJSTokenType::New;
    if (value == "this") return CHTLJSTokenType::This;
    if (value == "super") return CHTLJSTokenType::Super;
    if (value == "static") return CHTLJSTokenType::Static;
    if (value == "async") return CHTLJSTokenType::Async;
    if (value == "await") return CHTLJSTokenType::Await;
    if (value == "try") return CHTLJSTokenType::Try;
    if (value == "catch") return CHTLJSTokenType::Catch;
    if (value == "finally") return CHTLJSTokenType::Finally;
    if (value == "throw") return CHTLJSTokenType::Throw;
    if (value == "typeof") return CHTLJSTokenType::Typeof;
    if (value == "instanceof") return CHTLJSTokenType::Instanceof;
    if (value == "in") return CHTLJSTokenType::In;
    if (value == "of") return CHTLJSTokenType::Of;
    if (value == "with") return CHTLJSTokenType::With;
    if (value == "delete") return CHTLJSTokenType::Delete;
    if (value == "void") return CHTLJSTokenType::Void;
    if (value == "true") return CHTLJSTokenType::Boolean;
    if (value == "false") return CHTLJSTokenType::Boolean;
    if (value == "null") return CHTLJSTokenType::Null;
    if (value == "undefined") return CHTLJSTokenType::Undefined;
    
    // CHTL JS keywords
    if (value == "listen") return CHTLJSTokenType::Listen;
    if (value == "delegate") return CHTLJSTokenType::Delegate;
    if (value == "animate") return CHTLJSTokenType::Animate;
    if (value == "vir") return CHTLJSTokenType::Vir;
    if (value == "router") return CHTLJSTokenType::Router;
    if (value == "fileloader") return CHTLJSTokenType::FileLoader;
    if (value == "script") return CHTLJSTokenType::Script;
    if (value == "selector") return CHTLJSTokenType::Selector;
    if (value == "listener") return CHTLJSTokenType::Listener;
    
    return CHTLJSTokenType::Identifier;
}

bool CHTLJSLexer::isOperatorStart(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|';
}

bool CHTLJSLexer::isPunctuationStart(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
           c == ',' || c == ';' || c == ':' || c == '.' || c == '?';
}

bool CHTLJSLexer::isStringStart(char c) const {
    return c == '"' || c == '\'' || c == '`';
}

bool CHTLJSLexer::isNumberStart(char c) const {
    return isDigit(c);
}

bool CHTLJSLexer::isIdentifierStart(char c) const {
    return isAlpha(c);
}

bool CHTLJSLexer::isCommentStart(char c) const {
    return c == '/';
}

bool CHTLJSLexer::isCHTLJSStart(char c) const {
    return c == '@' || c == '#' || c == '$';
}

} // namespace CHTL