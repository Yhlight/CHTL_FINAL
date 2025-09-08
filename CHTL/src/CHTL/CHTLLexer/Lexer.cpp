#include "Lexer.h"
#include <cctype>
#include <map>

namespace CHTL {

// Keyword map for easy lookup
static std::map<std::string, TokenType> keywords = {
    {"delete", TokenType::KeywordDelete}
};

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}

Token Lexer::nextToken() {
    m_start = m_current;

    if (m_current >= m_source.length()) return makeToken(TokenType::EndOfFile);

    char c = advance();

    switch (c) {
        case ' ': case '\r': case '\t': return nextToken();
        case '\n': m_line++; return nextToken();
        case '{': return makeToken(TokenType::OpenBrace, "{");
        case '}': return makeToken(TokenType::CloseBrace, "}");
        case '=': return makeToken(TokenType::Equals, "=");
        case ';': return makeToken(TokenType::Semicolon, ";");
        case '.': return makeToken(TokenType::Dot, ".");
        case '#': return makeToken(TokenType::Hash, "#");
        case '[': return makeToken(TokenType::LeftBracket, "[");
        case ']': return makeToken(TokenType::RightBracket, "]");
        case '@': return makeToken(TokenType::At, "@");
        case '?': return makeToken(TokenType::QuestionMark, "?");
        case '>': return makeToken(TokenType::GreaterThan, ">");
        case '<': return makeToken(TokenType::LessThan, "<");
        case '+': return makeToken(TokenType::Plus, "+");
        case '-': return makeToken(TokenType::Minus, "-");
        case '*': return makeToken(TokenType::Star, "*");
        case ':': if (match(':')) return makeToken(TokenType::ColonColon, "::"); return makeToken(TokenType::Colon, ":");
        case '&': if (match('&')) return makeToken(TokenType::LogicalAnd, "&&"); return makeToken(TokenType::Ampersand, "&");
        case '|': if (match('|')) return makeToken(TokenType::LogicalOr, "||"); return makeToken(TokenType::Unexpected, "|");
        case '"': case '\'': return makeString(c);
        case '/':
            if (match('/')) { skipLineComment(); return nextToken(); }
            if (match('*')) { skipBlockComment(); return nextToken(); }
            return makeToken(TokenType::Slash, "/");
    }

    m_current--;
    return makeIdentifierOrUnquotedLiteral();
}

Token Lexer::makeIdentifierOrUnquotedLiteral() {
    m_start = m_current;
    while (m_current < m_source.length()) {
        char c = peek();
        if (std::isalnum(c) || c == '_') {
            advance();
        } else {
            break;
        }
    }
    std::string value = m_source.substr(m_start, m_current - m_start);

    // Check if the identifier is a keyword
    if (keywords.count(value)) {
        return makeToken(keywords.at(value), value);
    }

    return makeToken(TokenType::Identifier, value);
}

Token Lexer::makeString(char quote_type) {
    m_start = m_current;
    while (peek() != quote_type && m_current < m_source.length()) { if (peek() == '\n') m_line++; advance(); }
    if (m_current >= m_source.length()) return makeToken(TokenType::Unexpected, "Unterminated string");
    std::string value = m_source.substr(m_start, m_current - m_start);
    advance();
    return makeToken(TokenType::StringLiteral, value);
}

void Lexer::skipLineComment() { while (peek() != '\n' && m_current < m_source.length()) advance(); }
void Lexer::skipBlockComment() { while (m_current < m_source.length() - 1) { if (peek() == '*' && m_source[m_current + 1] == '/') { advance(); advance(); return; } if (peek() == '\n') m_line++; advance(); } }
void Lexer::skipGeneratorComment() { while (peek() != '\n' && m_current < m_source.length()) advance(); }
char Lexer::peek() const { if (m_current >= m_source.length()) return '\0'; return m_source[m_current]; }
char Lexer::advance() { return m_source[m_current++]; }
bool Lexer::match(char expected) { if (m_current >= m_source.length()) return false; if (m_source[m_current] != expected) return false; m_current++; return true; }
Token Lexer::makeToken(TokenType type, const std::string& value) const { return {type, value, m_line, 0}; }

} // namespace CHTL
