#include "../../include/CHTL/CHTLLexer.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace CHTL {

// Static member initialization
const std::unordered_map<std::string, TokenType> CHTLLexer::s_keywords = {
    // Basic keywords
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    
    // Template keywords
    {"Template", TokenType::TEMPLATE},
    {"Custom", TokenType::CUSTOM},
    {"Origin", TokenType::ORIGIN},
    {"Import", TokenType::IMPORT},
    {"Constraints", TokenType::CONSTRAINTS},
    {"Configuration", TokenType::CONFIGURATION},
    {"Namespace", TokenType::NAMESPACE},
    
    // Control keywords
    {"use", TokenType::USE},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::AT_TOP},  // "at top" will be handled specially
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"html5", TokenType::HTML5},
    
    // CHTL JS keywords
    {"fileloader", TokenType::FILELOADER},
    {"listen", TokenType::LISTEN},
    {"delegate", TokenType::DELEGATE},
    {"animate", TokenType::ANIMATE},
    {"vir", TokenType::VIR},
    {"router", TokenType::ROUTER},
    
    // Special keywords
    {"top", TokenType::AT_TOP},
    {"bottom", TokenType::AT_BOTTOM}
};

const std::unordered_map<std::string, TokenType> CHTLLexer::s_operators = {
    {"&&", TokenType::AND},
    {"||", TokenType::OR},
    {"!", TokenType::NOT},
    {"==", TokenType::EQUALS},
    {"!=", TokenType::NOT_EQUALS},
    {"<=", TokenType::LESS_EQUAL},
    {">=", TokenType::GREATER_EQUAL},
    {"+=", TokenType::PLUS_EQUAL},
    {"-=", TokenType::MINUS_EQUAL},
    {"*=", TokenType::MULTIPLY_EQUAL},
    {"/=", TokenType::DIVIDE_EQUAL},
    {"%=", TokenType::MODULO_EQUAL},
    {"->", TokenType::ARROW}
};

const std::unordered_map<std::string, TokenType> CHTLLexer::s_symbols = {
    {"{", TokenType::LEFT_BRACE},
    {"}", TokenType::RIGHT_BRACE},
    {"[", TokenType::LEFT_BRACKET},
    {"]", TokenType::RIGHT_BRACKET},
    {"(", TokenType::LEFT_PAREN},
    {")", TokenType::RIGHT_PAREN},
    {";", TokenType::SEMICOLON},
    {":", TokenType::COLON},
    {"=", TokenType::EQUAL},
    {",", TokenType::COMMA},
    {".", TokenType::DOT},
    {"#", TokenType::HASH},
    {"@", TokenType::AT},
    {"&", TokenType::AMPERSAND},
    {"?", TokenType::QUESTION},
    {"!", TokenType::EXCLAMATION},
    {"_", TokenType::UNDERSCORE},
    {"|", TokenType::PIPE},
    {"~", TokenType::TILDE},
    {"^", TokenType::CARET},
    {"$", TokenType::DOLLAR},
    {"%", TokenType::PERCENT},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::ASTERISK},
    {"/", TokenType::SLASH},
    {"\\", TokenType::BACKSLASH},
    {"<", TokenType::LESS_THAN},
    {">", TokenType::GREATER_THAN}
};

// Regex patterns
const std::regex CHTLLexer::s_identifierPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
const std::regex CHTLLexer::s_stringPattern(R"("([^"\\]|\\.)*")");
const std::regex CHTLLexer::s_numberPattern(R"(\d+(\.\d+)?([eE][+-]?\d+)?)");
const std::regex CHTLLexer::s_literalPattern(R"([a-zA-Z_][a-zA-Z0-9_\s]*)");

// Constructor
CHTLLexer::CHTLLexer(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_position(0), m_line(1), m_column(1) {
}

// Main tokenization function
std::vector<Token> CHTLLexer::tokenize(const std::string& source) {
    setSource(source);
    std::vector<Token> tokens;
    
    while (hasMoreTokens()) {
        Token token = getNextToken();
        if (token.type != TokenType::WHITESPACE) {
            tokens.push_back(token);
        }
    }
    
    // Add EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", m_line, m_column, m_position);
    
    return tokens;
}

// Get next token
Token CHTLLexer::getNextToken() {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", m_line, m_column, m_position);
    }
    
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", m_line, m_column, m_position);
    }
    
    char c = current();
    
    // Handle comments
    if (c == '/' && peek() == '/') {
        return scanLineComment();
    }
    if (c == '/' && peek() == '*') {
        return scanBlockComment();
    }
    if (c == '-' && peek() == '-') {
        return scanGeneratorComment();
    }
    
    // Handle strings
    if (c == '"' || c == '\'') {
        return scanString();
    }
    
    // Handle numbers
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // Handle identifiers and keywords
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // Handle operators and symbols
    if (isOperator(std::string(1, c)) || isSymbol(std::string(1, c))) {
        return scanOperator();
    }
    
    // Handle symbols
    return scanSymbol();
}

// Check if more tokens available
bool CHTLLexer::hasMoreTokens() const {
    return !isAtEnd();
}

// Reset lexer state
void CHTLLexer::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
    m_errors.clear();
}

// Set source code
void CHTLLexer::setSource(const std::string& source) {
    m_source = source;
    reset();
}

// Get errors
std::vector<std::string> CHTLLexer::getErrors() const {
    return m_errors;
}

// Check if has errors
bool CHTLLexer::hasErrors() const {
    return !m_errors.empty();
}

// Clear errors
void CHTLLexer::clearErrors() {
    m_errors.clear();
}

// Skip whitespace
void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(current())) {
        if (isNewline(current())) {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        advance();
    }
}

// Scan line comment
Token CHTLLexer::scanLineComment() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value = "//";
    advance(2); // Skip //
    
    while (!isAtEnd() && !isNewline(current())) {
        value += current();
        advance();
    }
    
    return Token(TokenType::LINE_COMMENT, value, startLine, startColumn, startPos);
}

// Scan block comment
Token CHTLLexer::scanBlockComment() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value = "/*";
    advance(2); // Skip /*
    
    while (!isAtEnd()) {
        if (current() == '*' && peek() == '/') {
            value += "*/";
            advance(2);
            break;
        }
        
        if (isNewline(current())) {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        
        value += current();
        advance();
    }
    
    if (isAtEnd()) {
        addError("Unterminated block comment", startLine, startColumn);
    }
    
    return Token(TokenType::BLOCK_COMMENT, value, startLine, startColumn, startPos);
}

// Scan generator comment
Token CHTLLexer::scanGeneratorComment() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value = "--";
    advance(2); // Skip --
    
    while (!isAtEnd() && !isNewline(current())) {
        value += current();
        advance();
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, startPos);
}

// Scan identifier
Token CHTLLexer::scanIdentifier() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(current()) || current() == '_')) {
        value += current();
        advance();
    }
    
    // Check if it's a keyword
    TokenType type = getKeywordType(value);
    if (type != TokenType::UNKNOWN) {
        return Token(type, value, startLine, startColumn, startPos);
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn, startPos);
}

// Scan string
Token CHTLLexer::scanString() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    char quote = current();
    std::string value;
    value += quote;
    advance(); // Skip opening quote
    
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\' && !isAtEnd()) {
            value += current();
            advance();
            if (!isAtEnd()) {
                value += current();
                advance();
            }
        } else {
            value += current();
            advance();
        }
    }
    
    if (isAtEnd()) {
        addError("Unterminated string literal", startLine, startColumn);
        return Token(TokenType::STRING, value, startLine, startColumn, startPos);
    }
    
    value += current();
    advance(); // Skip closing quote
    
    return Token(TokenType::STRING, value, startLine, startColumn, startPos);
}

// Scan number
Token CHTLLexer::scanNumber() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value;
    
    // Integer part
    while (!isAtEnd() && isDigit(current())) {
        value += current();
        advance();
    }
    
    // Decimal part
    if (!isAtEnd() && current() == '.') {
        value += current();
        advance();
        
        while (!isAtEnd() && isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    // Exponent part
    if (!isAtEnd() && (current() == 'e' || current() == 'E')) {
        value += current();
        advance();
        
        if (!isAtEnd() && (current() == '+' || current() == '-')) {
            value += current();
            advance();
        }
        
        while (!isAtEnd() && isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, startPos);
}

// Scan literal
Token CHTLLexer::scanLiteral() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value;
    
    while (!isAtEnd() && !isWhitespace(current()) && 
           !isSymbol(std::string(1, current())) && 
           current() != ';' && current() != ':' && current() != '=') {
        value += current();
        advance();
    }
    
    return Token(TokenType::LITERAL, value, startLine, startColumn, startPos);
}

// Scan operator
Token CHTLLexer::scanOperator() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value;
    
    // Try two-character operators first
    if (!isAtEnd()) {
        std::string twoChar = std::string(1, current()) + std::string(1, peek());
        if (isOperator(twoChar)) {
            value = twoChar;
            advance(2);
            return Token(getOperatorType(value), value, startLine, startColumn, startPos);
        }
    }
    
    // Single character operators
    value = current();
    advance();
    
    TokenType type = getOperatorType(value);
    if (type != TokenType::UNKNOWN) {
        return Token(type, value, startLine, startColumn, startPos);
    }
    
    return Token(TokenType::UNKNOWN, value, startLine, startColumn, startPos);
}

// Scan symbol
Token CHTLLexer::scanSymbol() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    size_t startPos = m_position;
    
    std::string value(1, current());
    advance();
    
    TokenType type = getSymbolType(value);
    if (type != TokenType::UNKNOWN) {
        return Token(type, value, startLine, startColumn, startPos);
    }
    
    return Token(TokenType::UNKNOWN, value, startLine, startColumn, startPos);
}

// Character utilities
char CHTLLexer::current() const {
    if (isAtEnd()) return '\0';
    return m_source[m_position];
}

char CHTLLexer::peek(size_t offset) const {
    if (m_position + offset >= m_source.length()) return '\0';
    return m_source[m_position + offset];
}

void CHTLLexer::advance(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (!isAtEnd()) {
            if (isNewline(current())) {
                m_line++;
                m_column = 1;
            } else {
                m_column++;
            }
            m_position++;
        }
    }
}

bool CHTLLexer::isAtEnd() const {
    return m_position >= m_source.length();
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n';
}

// Error reporting
void CHTLLexer::addError(const std::string& message) {
    addError(message, m_line, m_column);
}

void CHTLLexer::addError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "Lexer error at line " << line << ", column " << column << ": " << message;
    m_errors.push_back(ss.str());
}

// Static utility functions
std::string CHTLLexer::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LITERAL: return "LITERAL";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::AT: return "AT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        case TokenType::UNDERSCORE: return "UNDERSCORE";
        case TokenType::ARROW: return "ARROW";
        case TokenType::PIPE: return "PIPE";
        case TokenType::TILDE: return "TILDE";
        case TokenType::CARET: return "CARET";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::ASTERISK: return "ASTERISK";
        case TokenType::SLASH: return "SLASH";
        case TokenType::BACKSLASH: return "BACKSLASH";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::CONSTRAINTS: return "CONSTRAINTS";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::USE: return "USE";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::HTML5: return "HTML5";
        case TokenType::FILELOADER: return "FILELOADER";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::ROUTER: return "ROUTER";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::NOT_EQUALS: return "NOT_EQUALS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
        case TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
        case TokenType::MULTIPLY_EQUAL: return "MULTIPLY_EQUAL";
        case TokenType::DIVIDE_EQUAL: return "DIVIDE_EQUAL";
        case TokenType::MODULO_EQUAL: return "MODULO_EQUAL";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

bool CHTLLexer::isKeyword(const std::string& word) {
    return s_keywords.find(word) != s_keywords.end();
}

TokenType CHTLLexer::getKeywordType(const std::string& word) {
    auto it = s_keywords.find(word);
    return (it != s_keywords.end()) ? it->second : TokenType::UNKNOWN;
}

bool CHTLLexer::isOperator(const std::string& word) {
    return s_operators.find(word) != s_operators.end();
}

TokenType CHTLLexer::getOperatorType(const std::string& word) {
    auto it = s_operators.find(word);
    return (it != s_operators.end()) ? it->second : TokenType::UNKNOWN;
}

bool CHTLLexer::isSymbol(const std::string& word) {
    return s_symbols.find(word) != s_symbols.end();
}

TokenType CHTLLexer::getSymbolType(const std::string& word) {
    auto it = s_symbols.find(word);
    return (it != s_symbols.end()) ? it->second : TokenType::UNKNOWN;
}

// TokenStream implementation
TokenStream::TokenStream(const std::vector<Token>& tokens)
    : m_tokens(tokens), m_position(0) {
}

const Token& TokenStream::current() const {
    if (isAtEnd()) {
        static const Token eofToken(TokenType::EOF_TOKEN, "", 0, 0, 0);
        return eofToken;
    }
    return m_tokens[m_position];
}

const Token& TokenStream::peek(size_t offset) const {
    if (m_position + offset >= m_tokens.size()) {
        static const Token eofToken(TokenType::EOF_TOKEN, "", 0, 0, 0);
        return eofToken;
    }
    return m_tokens[m_position + offset];
}

const Token& TokenStream::advance() {
    if (!isAtEnd()) {
        m_position++;
    }
    return current();
}

bool TokenStream::hasMore() const {
    return m_position < m_tokens.size();
}

bool TokenStream::isAtEnd() const {
    return m_position >= m_tokens.size() || current().type == TokenType::EOF_TOKEN;
}

size_t TokenStream::position() const {
    return m_position;
}

void TokenStream::setPosition(size_t pos) {
    m_position = pos;
}

void TokenStream::reset() {
    m_position = 0;
}

bool TokenStream::check(TokenType type) const {
    return !isAtEnd() && current().type == type;
}

bool TokenStream::check(const std::vector<TokenType>& types) const {
    if (isAtEnd()) return false;
    return std::find(types.begin(), types.end(), current().type) != types.end();
}

bool TokenStream::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool TokenStream::match(const std::vector<TokenType>& types) {
    if (check(types)) {
        advance();
        return true;
    }
    return false;
}

const Token& TokenStream::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    
    // Error handling - for now just return current token
    // In a real implementation, this would throw an exception or report an error
    return current();
}

void TokenStream::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (current().type == TokenType::SEMICOLON) {
            advance();
            break;
        }
        
        switch (current().type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
            case TokenType::ORIGIN:
            case TokenType::IMPORT:
            case TokenType::CONSTRAINTS:
            case TokenType::CONFIGURATION:
            case TokenType::NAMESPACE:
            case TokenType::TEXT:
            case TokenType::STYLE:
            case TokenType::SCRIPT:
                return;
            default:
                advance();
                break;
        }
    }
}

} // namespace CHTL