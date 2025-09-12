#include "Lexer.h"
#include <cctype>

namespace CHTL
{
    Lexer::Lexer(std::string input)
        : m_input(std::move(input)), m_position(0), m_line(1), m_char('\0')
    {
        if (!m_input.empty()) {
            m_char = m_input[m_position];
        }
    }

    void Lexer::advance()
    {
        if (m_char == '\n') {
            m_line++;
        }
        m_position++;
        if (m_position >= m_input.length()) {
            m_char = '\0';
        } else {
            m_char = m_input[m_position];
        }
    }

    char Lexer::peek()
    {
        if (m_position + 1 >= m_input.length()) {
            return '\0';
        }
        return m_input[m_position + 1];
    }

    char Lexer::peekNext()
    {
        if (m_position + 2 >= m_input.length()) {
            return '\0';
        }
        return m_input[m_position + 2];
    }

    Token Lexer::makeToken(TokenType type, const std::string& literal)
    {
        return Token(type, literal, m_line);
    }

    void Lexer::skipWhitespace()
    {
        while (m_char != '\0' && std::isspace(m_char)) {
            advance();
        }
    }

    Token Lexer::identifier()
    {
        size_t start_pos = m_position;
        // CHTL identifiers can contain letters, numbers, and hyphens
        while (m_char != '\0' && (std::isalnum(m_char) || m_char == '-')) {
            advance();
        }
        return makeToken(TokenType::IDENTIFIER, m_input.substr(start_pos, m_position - start_pos));
    }

    Token Lexer::stringLiteral(char quoteType)
    {
        size_t start_pos = m_position + 1; // Skip the opening quote
        advance();
        while (m_char != '\0' && m_char != quoteType) {
            advance();
        }
        if (m_char == '\0') {
            return makeToken(TokenType::ILLEGAL, "Unterminated string");
        }
        std::string literal = m_input.substr(start_pos, m_position - start_pos);
        advance(); // Skip the closing quote
        return makeToken(TokenType::STRING, literal);
    }

    Token Lexer::numberLiteral()
    {
        size_t start_pos = m_position;
        while (m_char != '\0' && (std::isdigit(m_char) || m_char == '.')) {
            advance();
        }
        return makeToken(TokenType::NUMBER, m_input.substr(start_pos, m_position - start_pos));
    }

    Token Lexer::comment()
    {
        // Single line comment `//`
        if (peek() == '/') {
            size_t start_pos = m_position;
            while (m_char != '\0' && m_char != '\n') {
                advance();
            }
            return makeToken(TokenType::SINGLE_LINE_COMMENT, m_input.substr(start_pos, m_position - start_pos));
        }
        // Multi-line comment `/* */`
        if (peek() == '*') {
            size_t start_pos = m_position;
            advance(); // consume /
            advance(); // consume *
            while (m_char != '\0' && (m_char != '*' || peek() != '/')) {
                advance();
            }
            if (m_char == '\0') {
                 return makeToken(TokenType::ILLEGAL, "Unterminated multi-line comment");
            }
            advance(); // consume *
            advance(); // consume /
            return makeToken(TokenType::MULTI_LINE_COMMENT, m_input.substr(start_pos, m_position - start_pos));
        }
        return makeToken(TokenType::ILLEGAL, std::string(1, m_char));
    }

    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;
        while (m_char != '\0')
        {
            skipWhitespace();

            if (m_char == '\0') break;

            if (std::isalpha(m_char)) {
                tokens.push_back(identifier());
                continue;
            }
            if (std::isdigit(m_char)) {
                tokens.push_back(numberLiteral());
                continue;
            }

            switch (m_char)
            {
                case '{': tokens.push_back(makeToken(TokenType::LEFT_BRACE, "{")); advance(); break;
                case '}': tokens.push_back(makeToken(TokenType::RIGHT_BRACE, "}")); advance(); break;
                case '(': tokens.push_back(makeToken(TokenType::LEFT_PAREN, "(")); advance(); break;
                case ')': tokens.push_back(makeToken(TokenType::RIGHT_PAREN, ")")); advance(); break;
                case '[': tokens.push_back(makeToken(TokenType::LEFT_BRACKET, "[")); advance(); break;
                case ']': tokens.push_back(makeToken(TokenType::RIGHT_BRACKET, "]")); advance(); break;
                case ':': tokens.push_back(makeToken(TokenType::COLON, ":")); advance(); break;
                case '=': tokens.push_back(makeToken(TokenType::COLON, "=")); advance(); break; // CE Equivalence
                case ';': tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); advance(); break;
                case ',': tokens.push_back(makeToken(TokenType::COMMA, ",")); advance(); break;
                case '.': tokens.push_back(makeToken(TokenType::DOT, ".")); advance(); break;
                case '+': tokens.push_back(makeToken(TokenType::PLUS, "+")); advance(); break;
                case '*':
                    if (peek() == '*') {
                        advance();
                        advance();
                        tokens.push_back(makeToken(TokenType::POWER, "**"));
                    } else {
                        tokens.push_back(makeToken(TokenType::STAR, "*"));
                        advance();
                    }
                    break;
                case '%': tokens.push_back(makeToken(TokenType::PERCENT, "%")); advance(); break;
                case '&': tokens.push_back(makeToken(TokenType::AMPERSAND, "&")); advance(); break;
                case '"':
                case '\'':
                    tokens.push_back(stringLiteral(m_char));
                    break;
                case '/':
                    if (peek() == '/' || peek() == '*') {
                        tokens.push_back(comment());
                    } else {
                        tokens.push_back(makeToken(TokenType::SLASH, "/"));
                        advance();
                    }
                    break;
                case '-':
                    if (peek() == '-') { // Generator comment --
                        advance();
                        advance();
                        size_t start_pos = m_position;
                        while(m_char != '\0' && m_char != '\n') {
                            advance();
                        }
                        tokens.push_back(makeToken(TokenType::GENERATOR_COMMENT, m_input.substr(start_pos, m_position - start_pos)));

                    } else {
                        tokens.push_back(makeToken(TokenType::MINUS, "-"));
                        advance();
                    }
                    break;
                default:
                    tokens.push_back(makeToken(TokenType::ILLEGAL, std::string(1, m_char)));
                    advance();
                    break;
            }
        }

        tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
        return tokens;
    }
}
