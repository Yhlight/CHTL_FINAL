#include "ChtlJsLexer.h"
#include <cctype>

ChtlJsLexer::ChtlJsLexer(const std::string& source) : source(source) {}

char ChtlJsLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char ChtlJsLexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool ChtlJsLexer::isAtEnd() const {
    return current >= source.length();
}

std::vector<ChtlJsToken> ChtlJsLexer::tokenize() {
    std::vector<ChtlJsToken> tokens;
    size_t last_pos = 0;

    while (current < source.length()) {
        char c = peek();
        char n = peekNext();

        if (c == '{' && n == '{') {
            if (current > last_pos) tokens.push_back({ChtlJsTokenType::OTHER_CONTENT, source.substr(last_pos, current - last_pos)});
            tokens.push_back({ChtlJsTokenType::L_DOUBLE_BRACE, "{{"});
            current += 2;
            last_pos = current;
        } else if (c == '}' && n == '}') {
            if (current > last_pos) tokens.push_back({ChtlJsTokenType::OTHER_CONTENT, source.substr(last_pos, current - last_pos)});
            tokens.push_back({ChtlJsTokenType::R_DOUBLE_BRACE, "}}"});
            current += 2;
            last_pos = current;
        } else if (c == '-' && n == '>') {
            if (current > last_pos) tokens.push_back({ChtlJsTokenType::OTHER_CONTENT, source.substr(last_pos, current - last_pos)});
            tokens.push_back({ChtlJsTokenType::ARROW, "->"});
            current += 2;
            last_pos = current;
        } else if (c == '{') {
            if (current > last_pos) tokens.push_back({ChtlJsTokenType::OTHER_CONTENT, source.substr(last_pos, current - last_pos)});
            tokens.push_back({ChtlJsTokenType::LBRACE, "{"});
            current++;
            last_pos = current;
        } else if (c == '}') {
            if (current > last_pos) tokens.push_back({ChtlJsTokenType::OTHER_CONTENT, source.substr(last_pos, current - last_pos)});
            tokens.push_back({ChtlJsTokenType::RBRACE, "}"});
            current++;
            last_pos = current;
        }
        else {
            current++;
        }
    }

    if (current > last_pos) {
        tokens.push_back({ChtlJsTokenType::OTHER_CONTENT, source.substr(last_pos, current - last_pos)});
    }

    tokens.push_back({ChtlJsTokenType::END_OF_FILE, ""});
    return tokens;
}
