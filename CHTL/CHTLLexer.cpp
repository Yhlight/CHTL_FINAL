#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

const std::map<std::string, TokenType> Lexer::keywords_ = {
    {"text", TokenType::KEYWORD_TEXT},
    // Future keywords like 'style', 'script' etc. can be added here
    // if they need special handling distinct from normal identifiers.
};

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }
    tokens_.push_back({TokenType::END_OF_FILE, "", line_, (int)current_});
    return tokens_;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '=': addToken(TokenType::EQUALS); break;

        case '/':
            if (match('/')) {
                consumeLineComment();
            } else if (match('*')) {
                consumeBlockComment();
            } else {
                // Division operator might be used in style properties later.
                // For now, it's an unknown character in the main CHTL syntax.
                addToken(TokenType::UNKNOWN);
            }
            break;

        case '-':
            if (match('-')) {
                consumeGeneratorComment();
            } else {
                addToken(TokenType::UNKNOWN);
            }
            break;

        case '"':
        case '\'':
            consumeString();
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line_++;
            break;

        default:
            if (std::isalpha(c) || c == '_') {
                consumeIdentifier();
            } else {
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}

void Lexer::consumeIdentifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords_.find(text);
    if (it == keywords_.end()) {
        addToken(TokenType::IDENTIFIER);
    } else {
        addToken(it->second);
    }
}

void Lexer::consumeString() {
    // The opening quote is at `current_ - 1`
    char quote_type = source_[current_ - 1];
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') line_++;
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string.
        // We could add an error token here.
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::consumeLineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    // We don't add a token for this type of comment as it's ignored.
}

void Lexer::consumeBlockComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') line_++;
        advance();
    }

    if (isAtEnd()) {
        // Unterminated block comment.
        return;
    }

    // Consume the closing "*/"
    advance();
    advance();
    // Ignored comment, no token added.
}

void Lexer::consumeGeneratorComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    // Extract the content of the comment, without the leading '--'
    std::string value = source_.substr(start_ + 2, current_ - start_ - 2);
    addToken(TokenType::COMMENT, value);
}


bool Lexer::isAtEnd() {
    return current_ >= source_.length();
}

char Lexer::advance() {
    return source_[current_++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source_.substr(start_, current_ - start_);
    if (!literal.empty()) {
        text = literal;
    }
    tokens_.push_back({type, text, line_, (int)start_});
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

} // namespace CHTL
