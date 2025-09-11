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

Token CHTLLexer::CreateToken(TokenType type, std::string value) {
    return {type, std::move(value), m_line, m_column - value.length()};
}

Token CHTLLexer::HandleIdentifier() {
    std::string value;
    while (std::isalnum(Peek()) || Peek() == '_' || Peek() == '-') {
        value += Consume();
    }
    if (value == "text") return CreateToken(TokenType::Text, value);
    if (value == "style") return CreateToken(TokenType::Style, value);
    if (value == "script") return CreateToken(TokenType::Script, value);
    if (value == "delete") return CreateToken(TokenType::Delete, value);
    if (value == "insert") return CreateToken(TokenType::Insert, value);
    if (value == "after") return CreateToken(TokenType::After, value);
    if (value == "before") return CreateToken(TokenType::Before, value);
    if (value == "replace") return CreateToken(TokenType::Replace, value);
    if (value == "at") return CreateToken(TokenType::At, value);
    if (value == "top") return CreateToken(TokenType::Top, value);
    if (value == "bottom") return CreateToken(TokenType::Bottom, value);

    return CreateToken(TokenType::Identifier, value);
}

Token CHTLLexer::HandleNumber() {
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
    return CreateToken(TokenType::Number, value);
}

Token CHTLLexer::HandleStringLiteral() {
    std::string value;
    char quote = Consume(); // Consume opening quote
    while (Peek() != quote && Peek() != '\0') {
        value += Consume();
    }
    if (Peek() == quote) {
        Consume(); // Consume closing quote
    }
    return CreateToken(TokenType::StringLiteral, value);
}


Token CHTLLexer::GetNextToken() {
    ConsumeWhitespace();

    if (m_cursor >= m_source.length()) {
        return CreateToken(TokenType::EndOfFile, "");
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
            return CreateToken(TokenType::GeneratorComment, value);
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
        return CreateToken(TokenType::LineComment, value);
    }
    if (current == '/' && Peek(1) == '*') {
        std::string value;
        Consume(); // /
        Consume(); // *
        while(Peek() != '*' || Peek(1) != '/') {
             if (Peek() == '\0') return CreateToken(TokenType::Unexpected, value);
            value += Consume();
        }
        Consume(); // *
        Consume(); // /
        return CreateToken(TokenType::BlockComment, value);
    }

    // Punctuation and Operators
    switch (current) {
        case '{': return CreateToken(TokenType::OpenBrace, std::string(1, Consume()));
        case '}': return CreateToken(TokenType::CloseBrace, std::string(1, Consume()));
        case ':': return CreateToken(TokenType::Colon, std::string(1, Consume()));
        case ';': return CreateToken(TokenType::Semicolon, std::string(1, Consume()));
        case '(': return CreateToken(TokenType::OpenParen, std::string(1, Consume()));
        case ')': return CreateToken(TokenType::CloseParen, std::string(1, Consume()));
        case '[': return CreateToken(TokenType::OpenBracket, std::string(1, Consume()));
        case ']': return CreateToken(TokenType::CloseBracket, std::string(1, Consume()));
        case '@': return CreateToken(TokenType::At, std::string(1, Consume()));
        case '#': return CreateToken(TokenType::Hash, std::string(1, Consume()));
        case '.': return CreateToken(TokenType::Dot, std::string(1, Consume()));
        case ',': return CreateToken(TokenType::Comma, std::string(1, Consume()));
        case '&':
            if (Peek(1) == '&') { Consume(); Consume(); return CreateToken(TokenType::LogicalAnd, "&&"); }
            return CreateToken(TokenType::Ampersand, std::string(1, Consume()));
        case '|':
            if (Peek(1) == '|') { Consume(); Consume(); return CreateToken(TokenType::LogicalOr, "||"); }
            break; // Single '|' is not a valid token
        case '=':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::DoubleEquals, "=="); }
            return CreateToken(TokenType::Equals, std::string(1, Consume()));
        case '!':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::NotEquals, "!="); }
            break; // Single '!' is not a valid token
        case '>':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::GreaterThanOrEquals, ">="); }
            return CreateToken(TokenType::GreaterThan, std::string(1, Consume()));
        case '<':
            if (Peek(1) == '=') { Consume(); Consume(); return CreateToken(TokenType::LessThanOrEquals, "<="); }
            return CreateToken(TokenType::LessThan, std::string(1, Consume()));
        case '?': return CreateToken(TokenType::QuestionMark, std::string(1, Consume()));
        case '+': return CreateToken(TokenType::Plus, std::string(1, Consume()));
        case '-': return CreateToken(TokenType::Minus, std::string(1, Consume()));
        case '/': return CreateToken(TokenType::Slash, std::string(1, Consume()));
        case '%': return CreateToken(TokenType::Percent, std::string(1, Consume()));
        case '*':
            if (Peek(1) == '*') {
                Consume(); Consume();
                return CreateToken(TokenType::DoubleStar, "**");
            }
            return CreateToken(TokenType::Star, std::string(1, Consume()));
    }

    // If we get here, it's an unexpected token
    return CreateToken(TokenType::Unexpected, std::string(1, Consume()));
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
