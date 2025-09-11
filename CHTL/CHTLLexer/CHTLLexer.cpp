#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(std::string source) : m_source(std::move(source)) {}

char CHTLLexer::Peek(size_t offset) {
    if (m_cursor + offset >= m_source.length()) {
        return '\0';
    }
    return m_source[m_cursor + offset];
}

char CHTLLexer::Consume() {
    char currentChar = m_source[m_cursor++];
    if (currentChar == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
    return currentChar;
}

void CHTLLexer::ConsumeWhitespace() {
    while (std::isspace(Peek())) {
        Consume();
    }
}

Token CHTLLexer::CreateToken(TokenType type, std::string value, size_t pos) {
    return {type, std::move(value), m_line, m_column - value.length(), pos, value.length()};
}

Token CHTLLexer::HandleIdentifier() {
    size_t startPos = m_cursor;
    std::string value;
    while (std::isalnum(Peek()) || Peek() == '_' || Peek() == '-') {
        value += Consume();
    }
    if (value == "text") return CreateToken(TokenType::Text, value, startPos);
    if (value == "style") return CreateToken(TokenType::Style, value, startPos);
    if (value == "script") return CreateToken(TokenType::Script, value, startPos);
    if (value == "delete") return CreateToken(TokenType::Delete, value, startPos);
    if (value == "insert") return CreateToken(TokenType::Insert, value, startPos);
    if (value == "after") return CreateToken(TokenType::After, value, startPos);
    if (value == "before") return CreateToken(TokenType::Before, value, startPos);
    if (value == "replace") return CreateToken(TokenType::Replace, value, startPos);
    if (value == "top") return CreateToken(TokenType::Top, value, startPos);
    if (value == "bottom") return CreateToken(TokenType::Bottom, value, startPos);
    if (value == "from") return CreateToken(TokenType::From, value, startPos);

    return CreateToken(TokenType::Identifier, value, startPos);
}

Token CHTLLexer::HandleNumber() {
    size_t startPos = m_cursor;
    std::string value;
    while (std::isdigit(Peek())) {
        value += Consume();
    }
    if (Peek() == '.') {
        value += Consume(); // consume the '.'
        while (std::isdigit(Peek())) {
            value += Consume();
        }
    }
    return CreateToken(TokenType::Number, value, startPos);
}

Token CHTLLexer::HandleStringLiteral() {
    size_t startPos = m_cursor;
    std::string value;
    char quote = Consume(); // Consume opening quote
    while (Peek() != quote && Peek() != '\0') {
        value += Consume();
    }
    if (Peek() == quote) {
        Consume(); // Consume closing quote
    }
    return CreateToken(TokenType::StringLiteral, value, startPos);
}


Token CHTLLexer::GetNextToken() {
    ConsumeWhitespace();
    size_t startPos = m_cursor;

    if (m_cursor >= m_source.length()) {
        return CreateToken(TokenType::EndOfFile, "", startPos);
    }

    char current = Peek();

    // Identifiers (must start with a letter, _, or -) and Keywords
    if (std::isalpha(current) || current == '_' || current == '-') {
        // A quick check for "--" generator comments
        if (current == '-' && Peek(1) == '-') {
             std::string value;
            while(Peek() != '\n' && Peek() != '\0') {
                value += Consume();
            }
            return CreateToken(TokenType::GeneratorComment, value, startPos);
        }
        return HandleIdentifier();
    }

    // Numbers
    if (std::isdigit(current)) {
        return HandleNumber();
    }

    // String Literals
    if (current == '"' || current == '\'') {
        return HandleStringLiteral();
    }

    // Comments
    if (current == '/' && Peek(1) == '/') {
        std::string value;
        while(Peek() != '\n' && Peek() != '\0') {
            value += Consume();
        }
        return CreateToken(TokenType::LineComment, value, startPos);
    }
    if (current == '/' && Peek(1) == '*') {
        std::string value;
        Consume(); // /
        Consume(); // *
        while(Peek() != '*' || Peek(1) != '/') {
             if (Peek() == '\0') return CreateToken(TokenType::Unexpected, value, startPos);
            value += Consume();
        }
        Consume(); // *
        Consume(); // /
        return CreateToken(TokenType::BlockComment, value, startPos);
    }

    // Punctuation and Operators
    switch (current) {
        case '{': return CreateToken(TokenType::OpenBrace, std::string(1, Consume()), startPos);
        case '}': return CreateToken(TokenType::CloseBrace, std::string(1, Consume()), startPos);
        case ':': return CreateToken(TokenType::Colon, std::string(1, Consume()), startPos);
        case ';': return CreateToken(TokenType::Semicolon, std::string(1, Consume()), startPos);
        case '(': return CreateToken(TokenType::OpenParen, std::string(1, Consume()), startPos);
        case ')': return CreateToken(TokenType::CloseParen, std::string(1, Consume()), startPos);
        case '[': return CreateToken(TokenType::OpenBracket, std::string(1, Consume()), startPos);
        case ']': return CreateToken(TokenType::CloseBracket, std::string(1, Consume()), startPos);
        case '@': return CreateToken(TokenType::At, std::string(1, Consume()), startPos);
        case '#': return CreateToken(TokenType::Hash, std::string(1, Consume()), startPos);
        case '.': return CreateToken(TokenType::Dot, std::string(1, Consume()), startPos);
        case ',': return CreateToken(TokenType::Comma, std::string(1, Consume()), startPos);
        case '&':
            if (Peek(1) == '&') { Consume(); Consume(); return CreateToken(TokenType::LogicalAnd, "&&", startPos); }
            return CreateToken(TokenType::Ampersand, std::string(1, Consume()), startPos);
        case '|':
            if (Peek(1) == '|') { Consume(); Consume(); return CreateToken(TokenType::LogicalOr, "||", startPos); }
            break; // Single '|' is not a valid token
        case '=':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::DoubleEquals, "==", startPos); }
            return CreateToken(TokenType::Equals, std::string(1, Consume()), startPos);
        case '!':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::NotEquals, "!=", startPos); }
            break; // Single '!' is not a valid token
        case '>':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::GreaterThanOrEquals, ">=", startPos); }
            return CreateToken(TokenType::GreaterThan, std::string(1, Consume()), startPos);
        case '<':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::LessThanOrEquals, "<=", startPos); }
            return CreateToken(TokenType::LessThan, std::string(1, Consume()), startPos);
        case '?': return CreateToken(TokenType::QuestionMark, std::string(1, Consume()), startPos);
        case '+': return CreateToken(TokenType::Plus, std::string(1, Consume()), startPos);
        case '-': return CreateToken(TokenType::Minus, std::string(1, Consume()), startPos);
        case '/': return CreateToken(TokenType::Slash, std::string(1, Consume()), startPos);
        case '%': return CreateToken(TokenType::Percent, std::string(1, Consume()), startPos);
        case '*':
            if (Peek(1) == '*') {
                Consume(); Consume();
                return CreateToken(TokenType::DoubleStar, "**", startPos);
            }
            return CreateToken(TokenType::Star, std::string(1, Consume()), startPos);
    }

    // If we get here, it's an unexpected token
    return CreateToken(TokenType::Unexpected, std::string(1, Consume()), startPos);
}

std::vector<Token> CHTLLexer::Tokenize() {
    std::vector<Token> tokens;
    Token token = GetNextToken();
    while (token.type != TokenType::EndOfFile) {
        tokens.push_back(token);
        token = GetNextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}

} // namespace CHTL
