#include "CHTLLexer.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() : position(0), line(1), column(1) {}

std::vector<Token> CHTLLexer::tokenize(const std::string& source) {
    this->source = source;
    position = 0;
    line = 1;
    column = 1;
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) {
            break;
        }
        
        auto token = nextToken();
        if (token) {
            tokens.push_back(*token);
        }
    }
    
    // Add EndOfFile token
    tokens.emplace_back(TokenType::EndOfFile, "", line, column, position);
    
    return tokens;
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    source.clear();
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

char CHTLLexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLLexer::peekChar() const {
    if (position + 1 >= source.length()) {
        return '\0';
    }
    return source[position + 1];
}

void CHTLLexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    position++;
}

void CHTLLexer::skipWhitespace() {
    while (currentChar() != '\0' && std::isspace(currentChar()) && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipSingleLineComment() {
    while (currentChar() != '\0' && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipMultiLineComment() {
    while (currentChar() != '\0') {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(); // skip *
            advance(); // skip /
            return;
        }
        advance();
    }
    throw std::runtime_error("Unterminated multi-line comment");
}

std::string CHTLLexer::readString(char quote) {
    std::string result;
    advance(); // skip opening quote
    
    while (currentChar() != '\0' && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (currentChar() != '\0') {
                switch (currentChar()) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '\'': result += '\''; break;
                    default: result += currentChar(); break;
                }
                advance();
            }
        } else {
            result += currentChar();
            advance();
        }
    }
    
    if (currentChar() == quote) {
        advance(); // skip closing quote
    } else {
        throw std::runtime_error("Unterminated string literal");
    }
    
    return result;
}

std::string CHTLLexer::readGeneratorComment() {
    std::string result;
    advance(); // skip first -
    advance(); // skip second -
    
    while (currentChar() != '\0' && currentChar() != '\n') {
        result += currentChar();
        advance();
    }
    
    // Trim whitespace
    size_t start = 0;
    while (start < result.length() && std::isspace(result[start])) {
        start++;
    }
    size_t end = result.length();
    while (end > start && std::isspace(result[end - 1])) {
        end--;
    }
    
    return result.substr(start, end - start);
}

std::string CHTLLexer::readIdentifier() {
    std::string result;
    
    while (currentChar() != '\0' && (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string CHTLLexer::readNumber() {
    std::string result;
    
    while (currentChar() != '\0' && (std::isdigit(currentChar()) || currentChar() == '.')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

TokenType CHTLLexer::getKeywordTokenType(const std::string& identifier) {
    if (identifier == "text") return TokenType::Text;
    if (identifier == "style") return TokenType::Style;
    if (identifier == "script") return TokenType::Script;
    if (identifier == "Template") return TokenType::Template;
    if (identifier == "Custom") return TokenType::Custom;
    if (identifier == "Origin") return TokenType::Origin;
    if (identifier == "Import") return TokenType::Import;
    if (identifier == "Namespace") return TokenType::Namespace;
    if (identifier == "Configuration") return TokenType::Configuration;
    if (identifier == "use") return TokenType::Use;
    return TokenType::Identifier;
}

Token CHTLLexer::createToken(TokenType token_type, const std::string& value, size_t line, size_t column, size_t position) {
    return Token(token_type, value, line, column, position);
}

std::unique_ptr<Token> CHTLLexer::nextToken() {
    size_t start_pos = position;
    size_t start_line = line;
    size_t start_col = column;
    
    char ch = currentChar();
    
    switch (ch) {
        case '{':
            advance();
            return std::make_unique<Token>(createToken(TokenType::LeftBrace, "{", start_line, start_col, start_pos));
            
        case '}':
            advance();
            return std::make_unique<Token>(createToken(TokenType::RightBrace, "}", start_line, start_col, start_pos));
            
        case '[':
            advance();
            return std::make_unique<Token>(createToken(TokenType::LeftBracket, "[", start_line, start_col, start_pos));
            
        case ']':
            advance();
            return std::make_unique<Token>(createToken(TokenType::RightBracket, "]", start_line, start_col, start_pos));
            
        case '(':
            advance();
            return std::make_unique<Token>(createToken(TokenType::LeftParen, "(", start_line, start_col, start_pos));
            
        case ')':
            advance();
            return std::make_unique<Token>(createToken(TokenType::RightParen, ")", start_line, start_col, start_pos));
            
        case ':':
            advance();
            return std::make_unique<Token>(createToken(TokenType::Colon, ":", start_line, start_col, start_pos));
            
        case '=':
            advance();
            if (peekChar() == '=') {
                advance();
                return std::make_unique<Token>(createToken(TokenType::EqualEqual, "==", start_line, start_col, start_pos));
            }
            return std::make_unique<Token>(createToken(TokenType::Equal, "=", start_line, start_col, start_pos));
            
        case ';':
            advance();
            return std::make_unique<Token>(createToken(TokenType::Semicolon, ";", start_line, start_col, start_pos));
            
        case ',':
            advance();
            return std::make_unique<Token>(createToken(TokenType::Comma, ",", start_line, start_col, start_pos));
            
        case '.':
            advance();
            return std::make_unique<Token>(createToken(TokenType::Dot, ".", start_line, start_col, start_pos));
            
        case '/':
            if (peekChar() == '/') {
                advance(); // skip first /
                advance(); // skip second /
                skipSingleLineComment();
                return nullptr; // Skip comments
            } else if (peekChar() == '*') {
                advance(); // skip /
                advance(); // skip *
                skipMultiLineComment();
                return nullptr; // Skip comments
            }
            advance();
            return std::make_unique<Token>(createToken(TokenType::Slash, "/", start_line, start_col, start_pos));
            
        case '@':
            advance();
            return std::make_unique<Token>(createToken(TokenType::At, "@", start_line, start_col, start_pos));
            
        case '#':
            advance();
            return std::make_unique<Token>(createToken(TokenType::Hash, "#", start_line, start_col, start_pos));
            
        case '!':
            advance();
            if (peekChar() == '=') {
                advance();
                return std::make_unique<Token>(createToken(TokenType::NotEqual, "!=", start_line, start_col, start_pos));
            }
            return std::make_unique<Token>(createToken(TokenType::Exclamation, "!", start_line, start_col, start_pos));
            
        case '?':
            advance();
            return std::make_unique<Token>(createToken(TokenType::Question, "?", start_line, start_col, start_pos));
            
        case '&':
            advance();
            if (peekChar() == '&') {
                advance();
                return std::make_unique<Token>(createToken(TokenType::And, "&&", start_line, start_col, start_pos));
            }
            return std::make_unique<Token>(createToken(TokenType::Ampersand, "&", start_line, start_col, start_pos));
            
        case '|':
            advance();
            if (peekChar() == '|') {
                advance();
                return std::make_unique<Token>(createToken(TokenType::Or, "||", start_line, start_col, start_pos));
            }
            throw std::runtime_error("Unexpected character '|'");
            
        case '>':
            advance();
            if (peekChar() == '=') {
                advance();
                return std::make_unique<Token>(createToken(TokenType::GreaterEqual, ">=", start_line, start_col, start_pos));
            }
            return std::make_unique<Token>(createToken(TokenType::GreaterThan, ">", start_line, start_col, start_pos));
            
        case '<':
            advance();
            if (peekChar() == '=') {
                advance();
                return std::make_unique<Token>(createToken(TokenType::LessEqual, "<=", start_line, start_col, start_pos));
            }
            return std::make_unique<Token>(createToken(TokenType::LessThan, "<", start_line, start_col, start_pos));
            
        case '"':
            {
                std::string value = readString('"');
                return std::make_unique<Token>(createToken(TokenType::StringLiteral, value, start_line, start_col, start_pos));
            }
            
        case '\'':
            {
                std::string value = readString('\'');
                return std::make_unique<Token>(createToken(TokenType::StringLiteral, value, start_line, start_col, start_pos));
            }
            
        case '-':
            advance();
            if (peekChar() == '-') {
                std::string value = readGeneratorComment();
                return std::make_unique<Token>(createToken(TokenType::GeneratorComment, value, start_line, start_col, start_pos));
            }
            throw std::runtime_error("Unexpected character '-'");
            
        default:
            if (std::isalpha(ch) || ch == '_') {
                std::string identifier = readIdentifier();
                TokenType token_type = getKeywordTokenType(identifier);
                return std::make_unique<Token>(createToken(token_type, identifier, start_line, start_col, start_pos));
            } else if (std::isdigit(ch)) {
                std::string number = readNumber();
                return std::make_unique<Token>(createToken(TokenType::NumberLiteral, number, start_line, start_col, start_pos));
            } else if (std::isspace(ch)) {
                advance();
                return nullptr; // Skip whitespace
            } else {
                throw std::runtime_error("Unexpected character: " + std::string(1, ch) + " (ASCII: " + std::to_string(static_cast<int>(ch)) + ")");
            }
    }
}

} // namespace CHTL