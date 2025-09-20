#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int fragment_start_line = 1;

    while (!isAtEnd()) {
        if (peek() == '/' && current + 1 < source.length()) {
            if (source[current + 1] == '/') {
                // Single-line comment
                while (peek() != '\n' && !isAtEnd()) {
                    buffer += advance();
                }
                 if(!isAtEnd()) buffer += advance();
            } else if (source[current + 1] == '*') {
                // Multi-line comment
                buffer += advance(); // /
                buffer += advance(); // *
                while (!isAtEnd() && (peek() != '*' || (current + 1 < source.length() && source[current + 1] != '/'))) {
                    buffer += advance();
                }
                if (!isAtEnd()) buffer += advance(); // *
                if (!isAtEnd()) buffer += advance(); // /
            } else {
                buffer += advance();
            }
        } else if (source.substr(current, 8) == "[Origin]") {
            if (!buffer.empty()) {
                fragments.push_back({FragmentType::CHTL, buffer, fragment_start_line});
                buffer.clear();
            }
            fragment_start_line = line;
            handleOriginBlock(fragments);
        } else {
            buffer += advance();
        }
    }

    if (!buffer.empty()) {
        fragments.push_back({FragmentType::CHTL, buffer, fragment_start_line});
    }

    return fragments;
}

void CHTLUnifiedScanner::handleOriginBlock(std::vector<CodeFragment>& fragments) {
    int block_start_pos = current;
    int block_start_line = line;

    current += 8; // Skip "[Origin]"

    skipWhitespaceAndComments();

    if (peek() != '@') {
        current = block_start_pos;
        return;
    }
    advance(); // Consume '@'

    int type_start = current;
    while (!isAtEnd() && isalnum(peek())) {
        advance();
    }
    std::string type = source.substr(type_start, current - type_start);

    while (!isAtEnd() && peek() != '{') {
        if (peek() == ']') {
            advance();
            break;
        }
        advance();
    }

    skipWhitespaceAndComments();

    if (peek() != '{') {
        current = block_start_pos;
        return;
    }

    advance(); // Consume '{'
    int content_start_line = line;

    int content_start = current;
    int brace_level = 1;

    while (!isAtEnd() && brace_level > 0) {
        if (peek() == '{') {
            brace_level++;
        } else if (peek() == '}') {
            brace_level--;
        }

        if (brace_level > 0) {
            advance();
        }
    }

    std::string content = source.substr(content_start, current - content_start);

    size_t first = content.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        content = "";
    } else {
        size_t last = content.find_last_not_of(" \t\n\r");
        content = content.substr(first, (last - first + 1));
    }


    if (type == "Html") {
        fragments.push_back({FragmentType::HTML, content, content_start_line});
    } else {
        std::string full_block = source.substr(block_start_pos, current - block_start_pos + 1);
        fragments.push_back({FragmentType::CHTL, full_block, block_start_line});
    }

    if (!isAtEnd()) {
      advance(); // Consume '}'
    }
}


bool CHTLUnifiedScanner::isAtEnd() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    if (current < source.length()) {
        if (source[current] == '\n') {
            line++;
        }
        return source[current++];
    }
    return '\0';
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void CHTLUnifiedScanner::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (isspace(c)) {
            advance();
        } else if (c == '/' && current + 1 < source.length() && source[current + 1] == '/') {
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
        } else if (c == '/' && current + 1 < source.length() && source[current + 1] == '*') {
            advance(); // consume /
            advance(); // consume *
            while (!isAtEnd() && (peek() != '*' || (current + 1 < source.length() && source[current + 1] != '/'))) {
                advance();
            }
            if (!isAtEnd()) advance(); // consume *
            if (!isAtEnd()) advance(); // consume /
        } else {
            break;
        }
    }
}

void CHTLUnifiedScanner::findNextBlock(std::vector<CodeFragment>& fragments) {
}

} // namespace CHTL
