#include "Lexer.h"
#include <cctype>
#include <vector>

namespace CHTL {

Lexer::Lexer(const std::string& source)
    : m_source(source), m_position(0), m_line(1), m_column(1) {}

char Lexer::peek(size_t offset) const {
    if (m_position + offset >= m_source.length()) {
        return '\0';
    }
    return m_source[m_position + offset];
}

char Lexer::consume() {
    char current = peek();
    if (current == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
    m_position++;
    return current;
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, m_line, m_column};
}

Token Lexer::makeToken(TokenType type, char value) {
    return makeToken(type, std::string(1, value));
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        consume();
    }
}

Token Lexer::lexComment() {
    if (peek(0) == '/' && peek(1) == '/') {
        std::string comment_value;
        consume(); // /
        consume(); // /
        while (peek() != '\n' && peek() != '\0') {
            comment_value += consume();
        }
        return makeToken(TokenType::LineComment, comment_value);
    }

    if (peek(0) == '/' && peek(1) == '*') {
        std::string comment_value;
        consume(); // /
        consume(); // *
        while (peek(0) != '*' || peek(1) != '/') {
            if (peek() == '\0') {
                // Unterminated block comment
                return makeToken(TokenType::Unexpected, comment_value);
            }
            comment_value += consume();
        }
        consume(); // *
        consume(); // /
        return makeToken(TokenType::BlockComment, comment_value);
    }

    if (peek(0) == '-' && peek(1) == '-') {
        std::string comment_value;
        consume(); // -
        consume(); // -
        while (peek() != '\n' && peek() != '\0') {
            comment_value += consume();
        }
        return makeToken(TokenType::GeneratorComment, comment_value);
    }

    return makeToken(TokenType::Unexpected, "");
}

Token Lexer::lexIdentifierOrUnquotedLiteral() {
    std::string value;
    // CHTL literals can contain almost anything until a delimiter is hit.
    // Delimiters are whitespace, ;, {, }, :, =, (, ), .
    while (peek() != '\0' && !std::isspace(peek()) && peek() != ';' && peek() != '{' && peek() != '}' && peek() != ':' && peek() != '=' && peek() != '(' && peek() != ')' && peek() != '.') {
        value += consume();
    }
    return makeToken(TokenType::Identifier, value);
}

Token Lexer::lexStringLiteral() {
    char quote_type = consume(); // Consume opening quote
    std::string value;
    while (peek() != quote_type) {
        if (peek() == '\0') {
            // Unterminated string
            return makeToken(TokenType::Unexpected, value);
        }
        value += consume();
    }
    consume(); // Consume closing quote
    return makeToken(TokenType::StringLiteral, value);
}


Token Lexer::getNextToken() {
    skipWhitespace();

    char current = peek();

    if (current == '\0') {
        return makeToken(TokenType::EndOfFile, "");
    }

    // Comments
    if ((current == '/' && (peek(1) == '/' || peek(1) == '*')) || (current == '-' && peek(1) == '-')) {
        // The spec says // and /* */ are not recognized by the generator.
        // We will tokenize them and the parser will decide to ignore them.
        return lexComment();
    }

    if (current == '&' && peek(1) == '&') {
        consume();
        consume();
        return makeToken(TokenType::LogicalAnd, "&&");
    }
    if (current == '|' && peek(1) == '|') {
        consume();
        consume();
        return makeToken(TokenType::LogicalOr, "||");
    }

    // Punctuation
    switch (current) {
        case '{': return makeToken(TokenType::OpenBrace, consume());
        case '}': return makeToken(TokenType::CloseBrace, consume());
        case '(': return makeToken(TokenType::OpenParen, consume());
        case ')': return makeToken(TokenType::CloseParen, consume());
        case ':': return makeToken(TokenType::Colon, consume());
        case ';': return makeToken(TokenType::Semicolon, consume());
        case '=': return makeToken(TokenType::Equals, consume());
        case '.': return makeToken(TokenType::Dot, consume());
        case '#': return makeToken(TokenType::Hash, consume());
        case '&': return makeToken(TokenType::Ampersand, consume());
        case '>': return makeToken(TokenType::GreaterThan, consume());
        case '<': return makeToken(TokenType::LessThan, consume());
        case '?': return makeToken(TokenType::QuestionMark, consume());
    }

    // String Literals
    if (current == '"' || current == '\'') {
        return lexStringLiteral();
    }

    // Identifiers and Unquoted Literals
    // This is tricky because `color: red` means `red` is a literal, but `div {` means `div` is an identifier.
    // The lexer will treat them the same (Identifier token). The parser will differentiate based on context.
    if (std::isalpha(current) || current == '_' || current == '.') { // Start of an identifier/unquoted literal
        return lexIdentifierOrUnquotedLiteral();
    }

    // Fallback for unquoted literals that don't start with a letter, e.g. in `width: 100px;`
    // This is a simplification. A more robust implementation would be needed for complex cases.
    if (!std::isspace(current) && current != ';' && current != '{' && current != '}' && current != ':' && current != '=') {
         return lexIdentifierOrUnquotedLiteral();
    }


    return makeToken(TokenType::Unexpected, consume());
}

std::vector<Token> Lexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    while (token.type != TokenType::EndOfFile) {
        tokens.push_back(token);
        token = getNextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}

} // namespace CHTL
