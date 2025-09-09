#include "CHTL/CHTLLexer.h"
#include <fstream>
#include <sstream>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : m_position(0)
    , m_line(1)
    , m_column(1)
{
    initializeKeywords();
}

void CHTLLexer::initializeKeywords() {
    // 基础关键字
    m_keywords["text"] = TokenType::TEXT;
    m_keywords["style"] = TokenType::STYLE;
    m_keywords["script"] = TokenType::SCRIPT;
    
    // 模板关键字
    m_keywords["[Template]"] = TokenType::TEMPLATE;
    m_keywords["Template"] = TokenType::TEMPLATE;
    
    // 自定义关键字
    m_keywords["[Custom]"] = TokenType::CUSTOM;
    m_keywords["Custom"] = TokenType::CUSTOM;
    
    // 原始嵌入关键字
    m_keywords["[Origin]"] = TokenType::ORIGIN;
    m_keywords["Origin"] = TokenType::ORIGIN;
    
    // 导入关键字
    m_keywords["[Import]"] = TokenType::IMPORT;
    m_keywords["Import"] = TokenType::IMPORT;
    
    // 命名空间关键字
    m_keywords["[Namespace]"] = TokenType::NAMESPACE;
    m_keywords["Namespace"] = TokenType::NAMESPACE;
    
    // 配置关键字
    m_keywords["[Configuration]"] = TokenType::CONFIGURATION;
    m_keywords["Configuration"] = TokenType::CONFIGURATION;
    
    // 操作关键字
    m_keywords["use"] = TokenType::USE;
    m_keywords["inherit"] = TokenType::INHERIT;
    m_keywords["delete"] = TokenType::DELETE;
    m_keywords["insert"] = TokenType::INSERT;
    m_keywords["from"] = TokenType::FROM;
    m_keywords["as"] = TokenType::AS;
    m_keywords["except"] = TokenType::EXCEPT;
    
    // 特殊符号
    m_keywords["@"] = TokenType::AT;
    m_keywords["&"] = TokenType::AMPERSAND;
    m_keywords["?"] = TokenType::QUESTION;
    m_keywords["|"] = TokenType::PIPE;
    m_keywords["->"] = TokenType::ARROW;
    m_keywords["#"] = TokenType::HASH;
}

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    m_input = input;
    m_position = 0;
    m_line = 1;
    m_column = 1;
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        char c = currentChar();
        
        if (c == '/' && peekChar() == '/') {
            tokens.push_back(readLineComment());
        } else if (c == '/' && peekChar() == '*') {
            tokens.push_back(readBlockComment());
        } else if (c == '-' && peekChar() == '-') {
            tokens.push_back(readGeneratorComment());
        } else if (c == '"' || c == '\'') {
            tokens.push_back(readString(c));
        } else if (std::isdigit(c)) {
            tokens.push_back(readNumber());
        } else if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentifier());
        } else if (c == '{') {
            tokens.push_back(Token(TokenType::LBRACE, "{", m_line, m_column, m_position));
            advance();
        } else if (c == '}') {
            tokens.push_back(Token(TokenType::RBRACE, "}", m_line, m_column, m_position));
            advance();
        } else if (c == '(') {
            tokens.push_back(Token(TokenType::LPAREN, "(", m_line, m_column, m_position));
            advance();
        } else if (c == ')') {
            tokens.push_back(Token(TokenType::RPAREN, ")", m_line, m_column, m_position));
            advance();
        } else if (c == '[') {
            tokens.push_back(Token(TokenType::LBRACKET, "[", m_line, m_column, m_position));
            advance();
        } else if (c == ']') {
            tokens.push_back(Token(TokenType::RBRACKET, "]", m_line, m_column, m_position));
            advance();
        } else if (c == ';') {
            tokens.push_back(Token(TokenType::SEMICOLON, ";", m_line, m_column, m_position));
            advance();
        } else if (c == ',') {
            tokens.push_back(Token(TokenType::COMMA, ",", m_line, m_column, m_position));
            advance();
        } else if (c == '.') {
            tokens.push_back(Token(TokenType::DOT, ".", m_line, m_column, m_position));
            advance();
        } else if (c == ':') {
            tokens.push_back(Token(TokenType::COLON, ":", m_line, m_column, m_position));
            advance();
        } else if (c == '=') {
            tokens.push_back(Token(TokenType::EQUAL, "=", m_line, m_column, m_position));
            advance();
        } else if (c == '@') {
            tokens.push_back(Token(TokenType::AT, "@", m_line, m_column, m_position));
            advance();
        } else if (c == '&') {
            tokens.push_back(Token(TokenType::AMPERSAND, "&", m_line, m_column, m_position));
            advance();
        } else if (c == '?') {
            tokens.push_back(Token(TokenType::QUESTION, "?", m_line, m_column, m_position));
            advance();
        } else if (c == '|') {
            tokens.push_back(Token(TokenType::PIPE, "|", m_line, m_column, m_position));
            advance();
        } else if (c == '#' && peekChar() == '#') {
            tokens.push_back(Token(TokenType::HASH, "#", m_line, m_column, m_position));
            advance();
        } else if (c == '-' && peekChar() == '>') {
            tokens.push_back(Token(TokenType::ARROW, "->", m_line, m_column, m_position));
            advance();
            advance();
        } else if (c == '&' && peekChar() == '&') {
            tokens.push_back(Token(TokenType::AND, "&&", m_line, m_column, m_position));
            advance();
            advance();
        } else if (c == '|' && peekChar() == '|') {
            tokens.push_back(Token(TokenType::OR, "||", m_line, m_column, m_position));
            advance();
            advance();
        } else if (c == '!') {
            tokens.push_back(Token(TokenType::NOT, "!", m_line, m_column, m_position));
            advance();
        } else if (c == '>') {
            if (peekChar() == '=') {
                tokens.push_back(Token(TokenType::GE, ">=", m_line, m_column, m_position));
                advance();
                advance();
            } else {
                tokens.push_back(Token(TokenType::GT, ">", m_line, m_column, m_position));
                advance();
            }
        } else if (c == '<') {
            if (peekChar() == '=') {
                tokens.push_back(Token(TokenType::LE, "<=", m_line, m_column, m_position));
                advance();
                advance();
            } else {
                tokens.push_back(Token(TokenType::LT, "<", m_line, m_column, m_position));
                advance();
            }
        } else if (c == '=' && peekChar() == '=') {
            tokens.push_back(Token(TokenType::EQ, "==", m_line, m_column, m_position));
            advance();
            advance();
        } else if (c == '!' && peekChar() == '=') {
            tokens.push_back(Token(TokenType::NE, "!=", m_line, m_column, m_position));
            advance();
            advance();
        } else {
            // 尝试读取字面量
            tokens.push_back(readLiteral());
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", m_line, m_column, m_position));
    return tokens;
}

std::vector<Token> CHTLLexer::tokenizeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return tokenize(buffer.str());
}

const std::unordered_map<std::string, TokenType>& CHTLLexer::getKeywordMap() const {
    return m_keywords;
}

void CHTLLexer::addKeyword(const std::string& keyword, TokenType type) {
    m_keywords[keyword] = type;
}

void CHTLLexer::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
    m_input.clear();
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        if (currentChar() == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        advance();
    }
}

void CHTLLexer::skipLineComment() {
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipBlockComment() {
    advance(); // 跳过 /*
    advance();
    
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance();
            advance();
            break;
        }
        advance();
    }
}

void CHTLLexer::skipGeneratorComment() {
    advance(); // 跳过 --
    advance();
    
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

Token CHTLLexer::readIdentifier() {
    size_t start = m_position;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        advance();
    }
    
    std::string value = m_input.substr(start, m_position - start);
    
    // 检查是否是关键字
    auto it = m_keywords.find(value);
    if (it != m_keywords.end()) {
        return Token(it->second, value, m_line, m_column - value.length(), start);
    }
    
    return Token(TokenType::IDENTIFIER, value, m_line, m_column - value.length(), start);
}

Token CHTLLexer::readString(char delimiter) {
    advance(); // 跳过开始引号
    size_t start = m_position;
    
    while (!isAtEnd() && currentChar() != delimiter) {
        if (currentChar() == '\\' && peekChar() == delimiter) {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    std::string value = m_input.substr(start, m_position - start);
    advance(); // 跳过结束引号
    
    return Token(TokenType::STRING, value, m_line, m_column - value.length() - 2, start - 1);
}

Token CHTLLexer::readNumber() {
    size_t start = m_position;
    
    while (!isAtEnd() && (std::isdigit(currentChar()) || currentChar() == '.')) {
        advance();
    }
    
    std::string value = m_input.substr(start, m_position - start);
    return Token(TokenType::NUMBER, value, m_line, m_column - value.length(), start);
}

Token CHTLLexer::readLiteral() {
    size_t start = m_position;
    
    while (!isAtEnd() && !isWhitespace(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' &&
           currentChar() != ';' && currentChar() != ',' &&
           currentChar() != ':' && currentChar() != '=') {
        advance();
    }
    
    std::string value = m_input.substr(start, m_position - start);
    return Token(TokenType::LITERAL, value, m_line, m_column - value.length(), start);
}

Token CHTLLexer::readOperator() {
    char c = currentChar();
    advance();
    
    if (c == '&' && currentChar() == '&') {
        advance();
        return Token(TokenType::AND, "&&", m_line, m_column - 2, m_position - 2);
    } else if (c == '|' && currentChar() == '|') {
        advance();
        return Token(TokenType::OR, "||", m_line, m_column - 2, m_position - 2);
    } else if (c == '=' && currentChar() == '=') {
        advance();
        return Token(TokenType::EQ, "==", m_line, m_column - 2, m_position - 2);
    } else if (c == '!' && currentChar() == '=') {
        advance();
        return Token(TokenType::NE, "!=", m_line, m_column - 2, m_position - 2);
    } else if (c == '>' && currentChar() == '=') {
        advance();
        return Token(TokenType::GE, ">=", m_line, m_column - 2, m_position - 2);
    } else if (c == '<' && currentChar() == '=') {
        advance();
        return Token(TokenType::LE, "<=", m_line, m_column - 2, m_position - 2);
    }
    
    return Token(TokenType::UNKNOWN, std::string(1, c), m_line, m_column - 1, m_position - 1);
}

char CHTLLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return m_input[m_position];
}

char CHTLLexer::peekChar() const {
    if (m_position + 1 >= m_input.length()) return '\0';
    return m_input[m_position + 1];
}

void CHTLLexer::advance() {
    if (!isAtEnd()) {
        m_position++;
        m_column++;
    }
}

bool CHTLLexer::isAtEnd() const {
    return m_position >= m_input.length();
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

} // namespace CHTL