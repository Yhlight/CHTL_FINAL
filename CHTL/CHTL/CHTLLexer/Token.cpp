#include "Token.h"
#include <sstream>
#include <stdexcept>

namespace CHTL {

const std::map<std::string, TokenType> KEYWORDS = {
    // 模板相关
    {"[Template]", TokenType::TEMPLATE},
    {"[Custom]", TokenType::CUSTOM},
    {"[Origin]", TokenType::ORIGIN},
    {"[Import]", TokenType::IMPORT},
    {"[Namespace]", TokenType::NAMESPACE},
    {"[Configuration]", TokenType::CONFIGURATION},
    {"[Info]", TokenType::INFO},
    {"[Export]", TokenType::EXPORT},
    
    // CHTL关键字
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"use", TokenType::USE},
    {"except", TokenType::EXCEPT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"inherit", TokenType::INHERIT},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    
    // CHTL JS关键字
    {"fileloader", TokenType::FILELOADER},
    {"listen", TokenType::LISTEN},
    {"delegate", TokenType::DELEGATE},
    {"animate", TokenType::ANIMATE},
    {"vir", TokenType::VIR},
    {"router", TokenType::ROUTER},
    {"util", TokenType::UTIL},
    {"iNeverAway", TokenType::INEVERAWAY},
    
    // 位置关键字
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::AT},
    {"top", TokenType::AT_TOP},
    {"bottom", TokenType::AT_BOTTOM}
};

const std::map<std::string, TokenType> OPERATORS = {
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::MULTIPLY},
    {"/", TokenType::DIVIDE},
    {"%", TokenType::MODULO},
    {"&&", TokenType::AND},
    {"||", TokenType::OR},
    {"!", TokenType::NOT},
    {">", TokenType::GREATER},
    {"<", TokenType::LESS},
    {">=", TokenType::GREATER_EQUAL},
    {"<=", TokenType::LESS_EQUAL},
    {"==", TokenType::EQUAL_EQUAL},
    {"!=", TokenType::NOT_EQUAL},
    {"?", TokenType::QUESTION},
    {"=", TokenType::EQUAL},
    {"->", TokenType::ARROW}
};

const std::map<std::string, TokenType> DELIMITERS = {
    {"{", TokenType::LEFT_BRACE},
    {"}", TokenType::RIGHT_BRACE},
    {"[", TokenType::LEFT_BRACKET},
    {"]", TokenType::RIGHT_BRACKET},
    {"(", TokenType::LEFT_PAREN},
    {")", TokenType::RIGHT_PAREN},
    {";", TokenType::SEMICOLON},
    {",", TokenType::COMMA},
    {".", TokenType::DOT},
    {":", TokenType::COLON},
    {"@", TokenType::AT},
    {"#", TokenType::HASH},
    {"$", TokenType::DOLLAR},
    {"&", TokenType::AMPERSAND},
    {"|", TokenType::PIPE},
    {"~", TokenType::TILDE},
    {"^", TokenType::CARET}
};

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << tokenTypeToString(type) << ", \"" << value << "\", " 
        << line << ":" << column << ")";
    return oss.str();
}

bool Token::isKeyword() const {
    return KEYWORDS.find(value) != KEYWORDS.end();
}

bool Token::isOperator() const {
    return OPERATORS.find(value) != OPERATORS.end();
}

bool Token::isDelimiter() const {
    return DELIMITERS.find(value) != DELIMITERS.end();
}

bool Token::isLiteral() const {
    return type == TokenType::STRING || type == TokenType::NUMBER || type == TokenType::IDENTIFIER;
}

void TokenStream::addToken(const Token& token) {
    tokens.push_back(token);
}

Token TokenStream::peek(size_t offset) const {
    size_t pos = current + offset;
    if (pos >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens[pos];
}

Token TokenStream::consume() {
    if (current >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens[current++];
}

bool TokenStream::hasMore() const {
    return current < tokens.size();
}

size_t TokenStream::size() const {
    return tokens.size();
}

void TokenStream::reset() {
    current = 0;
}

size_t TokenStream::getCurrentPosition() const {
    return current;
}

bool TokenStream::match(TokenType type) const {
    return peek().type == type;
}

bool TokenStream::match(const std::string& value) const {
    return peek().value == value;
}

bool TokenStream::match(TokenType type, const std::string& value) const {
    Token t = peek();
    return t.type == type && t.value == value;
}

void TokenStream::throwError(const std::string& message) const {
    Token currentToken = peek();
    std::ostringstream oss;
    oss << "Error at line " << currentToken.line << ", column " << currentToken.column 
        << ": " << message;
    throw std::runtime_error(oss.str());
}

TokenType getTokenType(const std::string& value) {
    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return it->second;
    }
    
    it = OPERATORS.find(value);
    if (it != OPERATORS.end()) {
        return it->second;
    }
    
    it = DELIMITERS.find(value);
    if (it != DELIMITERS.end()) {
        return it->second;
    }
    
    return TokenType::IDENTIFIER;
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::AT: return "AT";
        case TokenType::HASH: return "HASH";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::PIPE: return "PIPE";
        case TokenType::TILDE: return "TILDE";
        case TokenType::CARET: return "CARET";
        case TokenType::ARROW: return "ARROW";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::USE: return "USE";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::FILELOADER: return "FILELOADER";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::ROUTER: return "ROUTER";
        case TokenType::UTIL: return "UTIL";
        case TokenType::INEVERAWAY: return "INEVERAWAY";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool isKeyword(const std::string& value) {
    return KEYWORDS.find(value) != KEYWORDS.end();
}

bool isOperator(const std::string& value) {
    return OPERATORS.find(value) != OPERATORS.end();
}

bool isDelimiter(const std::string& value) {
    return DELIMITERS.find(value) != DELIMITERS.end();
}

} // namespace CHTL