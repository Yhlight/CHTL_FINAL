#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

// Mapping of string keywords to their corresponding token types
const std::unordered_map<std::string, TokenType> CHTLLexer::s_keywords = {
    {"text", TokenType::TOKEN_KEYWORD_TEXT},
    {"style", TokenType::TOKEN_KEYWORD_STYLE},
    {"script", TokenType::TOKEN_KEYWORD_SCRIPT},
    {"inherit", TokenType::TOKEN_KEYWORD_INHERIT},
    {"delete", TokenType::TOKEN_KEYWORD_DELETE},
    {"insert", TokenType::TOKEN_KEYWORD_INSERT},
    {"after", TokenType::TOKEN_KEYWORD_AFTER},
    {"before", TokenType::TOKEN_KEYWORD_BEFORE},
    {"replace", TokenType::TOKEN_KEYWORD_REPLACE},
    {"from", TokenType::TOKEN_KEYWORD_FROM},
    {"as", TokenType::TOKEN_KEYWORD_AS},
    {"except", TokenType::TOKEN_KEYWORD_EXCEPT},
    {"use", TokenType::TOKEN_KEYWORD_USE},
    {"html5", TokenType::TOKEN_KEYWORD_HTML5},
    // "[Custom]" style keywords are handled separately
};

CHTLLexer::CHTLLexer(std::string input)
    : m_input(std::move(input)), m_position(0), m_readPosition(0), m_char(0), m_line(1), m_column(0) {
    readChar(); // Initialize the lexer
}

void CHTLLexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
    m_column++;
    if (m_char == '\n') {
        m_line++;
        m_column = 0;
    }
}

char CHTLLexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void CHTLLexer::skipWhitespace() {
    while (m_char == ' ' || m_char == '\t' || m_char == '\n' || m_char == '\r') {
        readChar();
    }
}

void CHTLLexer::skipLineComment() {
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
}

void CHTLLexer::skipBlockComment() {
    readChar(); // Consume '*'
    while (m_char != 0) {
        if (m_char == '*' && peekChar() == '/') {
            readChar(); // Consume '*'
            readChar(); // Consume '/'
            return;
        }
        readChar();
    }
}

Token CHTLLexer::readIdentifierOrKeyword() {
    size_t startPos = m_position;
    while (isalpha(m_char) || isdigit(m_char) || m_char == '_') {
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);

    auto it = s_keywords.find(literal);
    if (it != s_keywords.end()) {
        return Token(it->second, literal, m_line, m_column - literal.length());
    }
    return Token(TokenType::TOKEN_IDENTIFIER, literal, m_line, m_column - literal.length());
}

Token CHTLLexer::readString(char quoteType) {
    int startLine = m_line;
    int startCol = m_column;
    size_t startPos = m_position + 1;
    readChar(); // consume opening quote
    while (m_char != quoteType && m_char != 0) {
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);
    readChar(); // consume closing quote
    return Token(TokenType::TOKEN_STRING, literal, startLine, startCol);
}

Token CHTLLexer::readNumber() {
    int startLine = m_line;
    int startCol = m_column;
    size_t startPos = m_position;
    while (isdigit(m_char) || m_char == '.') {
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);
    return Token(TokenType::TOKEN_NUMBER, literal, startLine, startCol);
}

Token CHTLLexer::readHashComment() {
    int startLine = m_line;
    int startCol = m_column;
    size_t startPos = m_position;

    // # must be followed by a space to be a comment
    if (peekChar() != ' ') {
         return Token(TokenType::TOKEN_HASH, "#", startLine, startCol);
    }

    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);
    return Token(TokenType::TOKEN_COMMENT, literal, startLine, startCol);
}


Token CHTLLexer::readBracketedKeyword() {
    int startLine = m_line;
    int startCol = m_column;
    size_t startPos = m_position;
    while (m_char != ']' && m_char != 0) {
        readChar();
    }
    readChar(); // consume ']'
    std::string literal = m_input.substr(startPos, m_position - startPos);

    if (literal == "[Custom]") return Token(TokenType::TOKEN_KEYWORD_CUSTOM, literal, startLine, startCol);
    if (literal == "[Template]") return Token(TokenType::TOKEN_KEYWORD_TEMPLATE, literal, startLine, startCol);
    if (literal == "[Origin]") return Token(TokenType::TOKEN_KEYWORD_ORIGIN, literal, startLine, startCol);
    if (literal == "[Import]") return Token(TokenType::TOKEN_KEYWORD_IMPORT, literal, startLine, startCol);
    if (literal == "[Namespace]") return Token(TokenType::TOKEN_KEYWORD_NAMESPACE, literal, startLine, startCol);
    if (literal == "[Configuration]") return Token(TokenType::TOKEN_KEYWORD_CONFIGURATION, literal, startLine, startCol);
    if (literal == "[Info]") return Token(TokenType::TOKEN_KEYWORD_INFO, literal, startLine, startCol);
    if (literal == "[Export]") return Token(TokenType::TOKEN_KEYWORD_EXPORT, literal, startLine, startCol);
    if (literal == "[Name]") return Token(TokenType::TOKEN_KEYWORD_NAME, literal, startLine, startCol);
    if (literal == "[OriginType]") return Token(TokenType::TOKEN_KEYWORD_ORIGINTYPE, literal, startLine, startCol);

    return Token(TokenType::TOKEN_ILLEGAL, literal, startLine, startCol);
}


Token CHTLLexer::NextToken() {
    Token tok;
    skipWhitespace();

    int startLine = m_line;
    int startCol = m_column;

    switch (m_char) {
        case '=': tok = Token(TokenType::TOKEN_EQUAL, "=", startLine, startCol); break;
        case ':': tok = Token(TokenType::TOKEN_COLON, ":", startLine, startCol); break;
        case ';': tok = Token(TokenType::TOKEN_SEMICOLON, ";", startLine, startCol); break;
        case '(': tok = Token(TokenType::TOKEN_LPAREN, "(", startLine, startCol); break;
        case ')': tok = Token(TokenType::TOKEN_RPAREN, ")", startLine, startCol); break;
        case '{': tok = Token(TokenType::TOKEN_LBRACE, "{", startLine, startCol); break;
        case '}': tok = Token(TokenType::TOKEN_RBRACE, "}", startLine, startCol); break;
        case '[': return readBracketedKeyword();
        case ']': tok = Token(TokenType::TOKEN_RBRACKET, "]", startLine, startCol); break;
        case ',': tok = Token(TokenType::TOKEN_COMMA, ",", startLine, startCol); break;
        case '+': tok = Token(TokenType::TOKEN_PLUS, "+", startLine, startCol); break;
        case '-': tok = Token(TokenType::TOKEN_MINUS, "-", startLine, startCol); break;
        case '/':
            if (peekChar() == '/') {
                skipLineComment();
                return NextToken(); // Recursively get the next token after the comment
            } else if (peekChar() == '*') {
                skipBlockComment();
                return NextToken(); // Recursively get the next token
            } else {
                tok = Token(TokenType::TOKEN_SLASH, "/", startLine, startCol);
            }
            break;
        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = Token(TokenType::TOKEN_POWER, "**", startLine, startCol);
            } else {
                tok = Token(TokenType::TOKEN_STAR, "*", startLine, startCol);
            }
            break;
        case '%': tok = Token(TokenType::TOKEN_PERCENT, "%", startLine, startCol); break;
        case '#': return readHashComment();
        case '@': tok = Token(TokenType::TOKEN_AT, "@", startLine, startCol); break;
        case '?': tok = Token(TokenType::TOKEN_QUESTION, "?", startLine, startCol); break;
        case '&':
            if (peekChar() == '&') {
                readChar();
                tok = Token(TokenType::TOKEN_LOGICAL_AND, "&&", startLine, startCol);
            } else {
                tok = Token(TokenType::TOKEN_AMPERSAND, "&", startLine, startCol);
            }
            break;
        case '|':
            if (peekChar() == '|') {
                readChar();
                tok = Token(TokenType::TOKEN_LOGICAL_OR, "||", startLine, startCol);
            } else {
                tok = Token(TokenType::TOKEN_ILLEGAL, "|", startLine, startCol);
            }
            break;
        case '"':
        case '\'':
            return readString(m_char);
        case 0:
            tok = Token(TokenType::TOKEN_EOF, "", startLine, startCol);
            break;
        default:
            if (isalpha(m_char) || m_char == '_') {
                return readIdentifierOrKeyword();
            } else if (isdigit(m_char)) {
                return readNumber();
            } else {
                tok = Token(TokenType::TOKEN_ILLEGAL, std::string(1, m_char), startLine, startCol);
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
