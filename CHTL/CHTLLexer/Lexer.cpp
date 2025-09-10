#include "Lexer.h"
#include <vector>

// Initialize the static keyword map
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
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    {"[Info]", TokenType::KEYWORD_INFO},
    {"[Export]", TokenType::KEYWORD_EXPORT},
    {"[Name]", TokenType::KEYWORD_NAME},
    {"[OriginType]", TokenType::KEYWORD_ORIGIN_TYPE}
    // "at top" and "at bottom" are special cases with spaces, handled in the parser.
};

Lexer::Lexer(const std::string& input)
    : m_input(input), m_position(0), m_readPosition(0), m_ch(0), m_line(1) {
    readChar(); // Initialize the first character
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token tok;
    do {
        tok = nextToken();
        tokens.push_back(tok);
    } while (tok.type != TokenType::END_OF_FILE);
    return tokens;
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_ch = 0; // EOF
    } else {
        m_ch = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0; // EOF
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r') {
        if (m_ch == '\n') {
            m_line++;
        }
        readChar();
    }
}

Token Lexer::nextToken() {
    skipWhitespace();

    Token tok;
    std::string lexeme(1, m_ch);

    switch (m_ch) {
        case '{': tok = newToken(TokenType::LEFT_BRACE, lexeme); break;
        case '}': tok = newToken(TokenType::RIGHT_BRACE, lexeme); break;
        case '(': tok = newToken(TokenType::LEFT_PAREN, lexeme); break;
        case ')': tok = newToken(TokenType::RIGHT_PAREN, lexeme); break;
        case '[': tok = newToken(TokenType::LEFT_BRACKET, lexeme); break;
        case ']': tok = newToken(TokenType::RIGHT_BRACKET, lexeme); break;
        case ':': tok = newToken(TokenType::COLON, lexeme); break;
        case ';': tok = newToken(TokenType::SEMICOLON, lexeme); break;
        case ',': tok = newToken(TokenType::COMMA, lexeme); break;
        case '.': tok = newToken(TokenType::DOT, lexeme); break;
        case '#': tok = newToken(TokenType::HASH, lexeme); break;
        case '$': tok = newToken(TokenType::DOLLAR, lexeme); break;
        case '@': tok = newToken(TokenType::AT, lexeme); break;
        case '?': tok = newToken(TokenType::QUESTION_MARK, lexeme); break;
        case '+': tok = newToken(TokenType::PLUS, lexeme); break;
        case '%': tok = newToken(TokenType::PERCENT, lexeme); break;
        case '=': tok = newToken(TokenType::EQUALS, lexeme); break;
        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = newToken(TokenType::POWER, "**");
            } else {
                tok = newToken(TokenType::ASTERISK, lexeme);
            }
            break;
        case '-':
            if (peekChar() == '-') {
                tok = readComment();
            } else {
                tok = newToken(TokenType::MINUS, lexeme);
            }
            break;
        case '/':
            if (peekChar() == '/' || peekChar() == '*') {
                tok = readComment();
            } else {
                tok = newToken(TokenType::SLASH, lexeme);
            }
            break;
        case '&':
            if (peekChar() == '&') {
                readChar();
                return newToken(TokenType::LOGICAL_AND, "&&");
            }
            return newToken(TokenType::AMPERSAND, lexeme);

        case '|':
            if (peekChar() == '|') {
                readChar();
                return newToken(TokenType::LOGICAL_OR, "||");
            }
            // Fallthrough to illegal for single '|'

        case '"':
        case '\'':
            return readStringLiteral();

        case 0: // End of File
            tok = newToken(TokenType::END_OF_FILE, "");
            break;

        default:
            if (isLetter(m_ch) || m_ch == '_') {
                return readIdentifier();
            } else if (isDigit(m_ch)) {
                return readNumberLiteral();
            } else {
                tok = newToken(TokenType::ILLEGAL, lexeme);
            }
            break;
    }

    readChar();
    return tok;
}


Token Lexer::readIdentifier() {
    size_t startPos = m_position;
    // Handle bracketed keywords like [Custom]
    if (m_ch == '[') {
        while (m_ch != ']' && m_ch != 0) {
            readChar();
        }
        if (m_ch == ']') readChar(); // consume ']'
    } else {
         while (isLetter(m_ch) || isDigit(m_ch) || m_ch == '_') {
            readChar();
        }
    }

    std::string lexeme = m_input.substr(startPos, m_position - startPos);

    // Check if the lexeme is a keyword
    auto it = s_keywords.find(lexeme);
    if (it != s_keywords.end()) {
        return newToken(it->second, lexeme);
    }

    return newToken(TokenType::IDENTIFIER, lexeme);
}

Token Lexer::readStringLiteral() {
    char quote_type = m_ch;
    size_t startPos = m_position + 1;
    readChar(); // consume opening quote

    while (m_ch != quote_type && m_ch != 0) {
        // Handle escaped quotes, e.g., "hello \" world"
        if (m_ch == '\\' && peekChar() == quote_type) {
            readChar();
        }
        readChar();
    }

    std::string lexeme = m_input.substr(startPos, m_position - startPos);

    if (m_ch == quote_type) {
        readChar(); // consume closing quote
    } else {
        // This is an unterminated string, return illegal
        return newToken(TokenType::ILLEGAL, lexeme);
    }

    return newToken(TokenType::STRING_LITERAL, lexeme);
}

Token Lexer::readNumberLiteral() {
    size_t startPos = m_position;
    while (isDigit(m_ch)) {
        readChar();
    }
    // Basic float support
    if (m_ch == '.' && isDigit(peekChar())) {
        readChar();
        while (isDigit(m_ch)) {
            readChar();
        }
    }
    std::string lexeme = m_input.substr(startPos, m_position - startPos);
    return newToken(TokenType::NUMBER_LITERAL, lexeme);
}

Token Lexer::readComment() {
    size_t startPos = m_position;
    char first_char = m_ch;
    char second_char = peekChar();

    if (first_char == '-' && second_char == '-') {
        while (m_ch != '\n' && m_ch != 0) {
            readChar();
        }
        std::string lexeme = m_input.substr(startPos, m_position - startPos);
        return newToken(TokenType::GENERATOR_COMMENT, lexeme);
    }

    if (first_char == '/' && second_char == '/') {
        while (m_ch != '\n' && m_ch != 0) {
            readChar();
        }
        std::string lexeme = m_input.substr(startPos, m_position - startPos);
        return newToken(TokenType::SINGLE_LINE_COMMENT, lexeme);
    }

    if (first_char == '/' && second_char == '*') {
        readChar(); // consume '/'
        readChar(); // consume '*'
        while (!(m_ch == '*' && peekChar() == '/') && m_ch != 0) {
            if (m_ch == '\n') m_line++;
            readChar();
        }
        if (m_ch != 0) {
            readChar(); // consume '*'
            readChar(); // consume '/'
        }
        std::string lexeme = m_input.substr(startPos, m_position - startPos);
        return newToken(TokenType::MULTI_LINE_COMMENT, lexeme);
    }

    // This part should not be reached given the logic in nextToken()
    return newToken(TokenType::ILLEGAL, std::string(1, m_ch));
}


bool Lexer::isLetter(char ch) const {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

bool Lexer::isDigit(char ch) const {
    return '0' <= ch && ch <= '9';
}

Token Lexer::newToken(TokenType type, const std::string& lexeme) const {
    return Token{type, lexeme, m_line};
}
