#include "Lexer.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <cctype>

namespace CHTL {

const std::map<std::string, TokenType> Lexer::s_keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"use", TokenType::KEYWORD_USE},
    {"html5", TokenType::KEYWORD_HTML5},
    // "at top" and "at bottom" are multi-word, will be handled by the parser
};

const std::map<std::string, TokenType> Lexer::s_bracketed_keywords = {
    {"Custom", TokenType::KEYWORD_CUSTOM},
    {"Template", TokenType::KEYWORD_TEMPLATE},
    {"Origin", TokenType::KEYWORD_ORIGIN},
    {"Import", TokenType::KEYWORD_IMPORT},
    {"Namespace", TokenType::KEYWORD_NAMESPACE},
    {"Configuration", TokenType::KEYWORD_CONFIGURATION},
    {"Info", TokenType::KEYWORD_INFO},
    {"Export", TokenType::KEYWORD_EXPORT},
    {"Name", TokenType::KEYWORD_NAME},
    {"OriginType", TokenType::KEYWORD_ORIGINTYPE},
};

const std::map<std::string, TokenType> Lexer::s_type_prefixes = {
    {"Style", TokenType::TYPE_STYLE},
    {"Element", TokenType::TYPE_ELEMENT},
    {"Var", TokenType::TYPE_VAR},
    {"Html", TokenType::TYPE_HTML},
    {"JavaScript", TokenType::TYPE_JAVASCRIPT},
    {"Chtl", TokenType::TYPE_CHTL},
    {"CJmod", TokenType::TYPE_CJMOD},
    {"Config", TokenType::TYPE_CONFIG},
    {"Vue", TokenType::TYPE_VUE},
};


Lexer::Lexer(const std::string& source) : m_source(source) {}

char Lexer::peek() const {
    if (m_current >= m_source.length()) return '\0';
    return m_source[m_current];
}

char Lexer::advance() {
    m_column++;
    return m_source[m_current++];
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                m_line++;
                m_column = 0; // reset column before advance
                advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::skipLineComment() {
    while (peek() != '\n' && peek() != '\0') {
        advance();
    }
}

void Lexer::skipBlockComment() {
    advance(); // Consume '*'
    advance(); // Consume '/'
    while (m_current + 1 < m_source.length() && (m_source[m_current] != '*' || m_source[m_current + 1] != '/')) {
        if (peek() == '\n') {
            m_line++;
            m_column = 0;
        }
        advance();
    }
    if (m_current + 1 < m_source.length()) {
        advance(); // Consume '*'
        advance(); // Consume '/'
    }
}


Token Lexer::readStringLiteral(char quote_type) {
    m_start = m_current;
    while (peek() != quote_type && peek() != '\0') {
        if (peek() == '\n') {
            m_line++;
            m_column = 0;
        }
        // handle escape characters
        if (peek() == '\\' && m_current + 1 < m_source.length()) {
            advance();
        }
        advance();
    }

    if (peek() == '\0') {
        return {TokenType::ILLEGAL, "Unterminated string", m_line, m_column};
    }

    advance(); // consume closing quote
    return {TokenType::STRING_LITERAL, m_source.substr(m_start, m_current - m_start - 1), m_line, m_column};
}

Token Lexer::readIdentifierOrKeyword() {
    m_start = m_current - 1;
    // Allow more characters in identifiers for unquoted literals like 'line-height' or '1.6'
    while (isalnum(peek()) || peek() == '_' || peek() == '-' || peek() == '.') {
        advance();
    }

    std::string text = m_source.substr(m_start, m_current - m_start);
    auto it = s_keywords.find(text);
    if (it != s_keywords.end()) {
        return {it->second, text, m_line, m_column - (int)text.length()};
    }

    // For now, unquoted literals are treated as identifiers. The parser will distinguish them.
    return {TokenType::IDENTIFIER, text, m_line, m_column - (int)text.length()};
}

Token Lexer::readBracketedKeyword() {
    m_start = m_current;
    while (peek() != ']' && peek() != '\0') {
        advance();
    }
    if (peek() == '\0') {
        return {TokenType::ILLEGAL, "Unterminated bracketed keyword", m_line, m_column};
    }

    std::string text = m_source.substr(m_start, m_current - m_start);
    advance(); // consume ']'

    auto it = s_bracketed_keywords.find(text);
    if (it != s_bracketed_keywords.end()) {
        return {it->second, "[" + text + "]", m_line, m_column - (int)text.length() - 2};
    }

    return {TokenType::ILLEGAL, "Unknown bracketed keyword: " + text, m_line, m_column - (int)text.length() - 2};
}


std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    m_start = m_current;

    char c = advance();

    if (c == '\0') return {TokenType::END_OF_FILE, "", m_line, m_column};

    if (isalpha(c) || c == '_') return readIdentifierOrKeyword();
    if (isdigit(c)) {
        return readIdentifierOrKeyword();
    }

    switch (c) {
        case '{': return {TokenType::LBRACE, "{", m_line, m_column - 1};
        case '}': return {TokenType::RBRACE, "}", m_line, m_column - 1};
        case '(': return {TokenType::LPAREN, "(", m_line, m_column - 1};
        case ')': return {TokenType::RPAREN, ")", m_line, m_column - 1};
        case '[': return readBracketedKeyword();
        case ']': return {TokenType::RBRACKET, "]", m_line, m_column - 1};
        case ':': return {TokenType::COLON, ":", m_line, m_column - 1};
        case ';': return {TokenType::SEMICOLON, ";", m_line, m_column - 1};
        case ',': return {TokenType::COMMA, ",", m_line, m_column - 1};
        case '.': return {TokenType::DOT, ".", m_line, m_column - 1};
        case '#': return {TokenType::HASH, "#", m_line, m_column - 1};
        case '?': return {TokenType::QUESTION, "?", m_line, m_column - 1};
        case '>': return {TokenType::GT, ">", m_line, m_column - 1};
        case '<': return {TokenType::LT, "<", m_line, m_column - 1};
        case '+': return {TokenType::PLUS, "+", m_line, m_column - 1};
        case '*': return {TokenType::ASTERISK, "*", m_line, m_column - 1};

        case '=': return {TokenType::EQUAL, "=", m_line, m_column - 1};

        case '"':
        case '\'':
            return readStringLiteral(c);

        case '@': {
            m_start = m_current;
            while (isalpha(peek())) {
                advance();
            }
            std::string text = m_source.substr(m_start, m_current - m_start);
            auto it = s_type_prefixes.find(text);
            if (it != s_type_prefixes.end()) {
                return {it->second, "@" + text, m_line, m_column - (int)text.length() -1};
            }
            return {TokenType::AT, "@", m_line, m_column -1};
        }

        case '&':
            if (peek() == '&') {
                advance();
                return {TokenType::LOGICAL_AND, "&&", m_line, m_column - 2};
            }
            return {TokenType::AMPERSAND, "&", m_line, m_column - 1};

        case '|':
            if (peek() == '|') {
                advance();
                return {TokenType::LOGICAL_OR, "||", m_line, m_column - 2};
            }
            break; // Fall through to illegal

        case '-':
            if (peek() == '-') {
                advance(); // consume second '-'
                skipLineComment(); // Treat as a line comment
                return nextToken();
            }
            return {TokenType::MINUS, "-", m_line, m_column-1};

        case '/':
            if (peek() == '/') {
                advance();
                skipLineComment();
                return nextToken(); // Get next token after comment
            }
            if (peek() == '*') {
                skipBlockComment();
                return nextToken(); // Get next token after comment
            }
            return {TokenType::SLASH, "/", m_line, m_column-1};
    }

    return {TokenType::ILLEGAL, std::string(1, c), m_line, m_column - 1};
}

} // namespace CHTL
