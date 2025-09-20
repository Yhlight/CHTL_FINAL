#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    while (!isAtEnd()) {
        scanToken();
    }
    return fragments;
}

void CHTLUnifiedScanner::scanToken() {
    switch (state) {
        case ScannerState::SCANNING_CHTL:
            scanChtl();
            break;
        case ScannerState::SCANNING_SCRIPT:
            scanScript();
            break;
        case ScannerState::SCANNING_CSS:
            scanStyle();
            break;
    }
}

void CHTLUnifiedScanner::scanChtl() {
    size_t start = current;
    while (!isAtEnd()) {
        if (peek() == '<') { // A simplistic check for tags, could be improved
            if (match("<style")) {
                addFragment(FragmentType::CHTL, source.substr(start, current - start - 6));
                state = ScannerState::SCANNING_CSS;
                return;
            } else if (match("<script")) {
                addFragment(FragmentType::CHTL, source.substr(start, current - start - 7));
                state = ScannerState::SCANNING_SCRIPT;
                return;
            }
        }

        if(match("style")) {
             // Lookahead to confirm it's a block and not just a property name
            int lookahead = current;
            while(lookahead < source.length() && isspace(source[lookahead])) {
                lookahead++;
            }
            if (lookahead < source.length() && source[lookahead] == '{') {
                addFragment(FragmentType::CHTL, source.substr(start, current - start - 5));
                state = ScannerState::SCANNING_CSS;
                return;
            }
        } else if(match("script")) {
            int lookahead = current;
            while(lookahead < source.length() && isspace(source[lookahead])) {
                lookahead++;
            }
            if (lookahead < source.length() && source[lookahead] == '{') {
                addFragment(FragmentType::CHTL, source.substr(start, current - start - 6));
                state = ScannerState::SCANNING_SCRIPT;
                return;
            }
        }


        advance();
    }

    // Add the final CHTL fragment
    if (current > start) {
        addFragment(FragmentType::CHTL, source.substr(start, current - start));
    }
}

void CHTLUnifiedScanner::scanScript() {
    std::string buffer;

    // Find the opening brace of the script block
    while (!isAtEnd() && peek() != '{') {
        advance();
    }
    if (isAtEnd()) return; // Should not happen with valid syntax
    advance(); // Consume '{'

    int brace_level = 1;

    while (!isAtEnd() && brace_level > 0) {
        skipWhitespace();

        // Check for CHTL JS enhanced selector {{...}}
        if (peek() == '{' && peekNext() == '{') {
            if (!buffer.empty()) {
                addFragment(FragmentType::JS, buffer);
                buffer.clear();
            }

            size_t cjs_start = current;
            advance(); advance(); // consume {{
            while (!isAtEnd() && !(peek() == '}' && peekNext() == '}')) {
                advance();
            }
            advance(); advance(); // consume }}

            // Lookahead for -> operator
            size_t after_braces = current;
            skipWhitespace();
            if (peek() == '-' && peekNext() == '>') {
                current = after_braces; // backtrack to include whitespace
                advance(); advance(); // consume ->
                while(!isAtEnd() && peek() != ';') {
                    advance();
                }
                advance(); // consume ;
            } else {
                current = after_braces; // backtrack
            }

            addFragment(FragmentType::CHTL_JS, source.substr(cjs_start, current - cjs_start));
            continue;
        }

        size_t word_start = current;
        if (isalpha(peek())) {
            while (isalpha(peek()) || isdigit(peek())) {
                advance();
            }
            std::string word = source.substr(word_start, current - word_start);
            if (isChtlJsKeyword(word)) {
                if (!buffer.empty()) {
                    addFragment(FragmentType::JS, buffer);
                    buffer.clear();
                }

                size_t cjs_start = word_start;
                int inner_brace_level = 0;
                // Find the opening brace if it exists
                while(!isAtEnd() && peek() != '{' && peek() != ';') {
                    advance();
                }

                if (peek() == '{') { // It's a block
                    inner_brace_level = 1;
                    advance();
                    while(!isAtEnd() && inner_brace_level > 0) {
                        if(peek() == '{') inner_brace_level++;
                        if(peek() == '}') inner_brace_level--;
                        advance();
                    }
                    // also consume the trailing ;
                    if(!isAtEnd() && peek() == ';') advance();

                } else { // It's a single statement
                     advance(); // consume ;
                }

                addFragment(FragmentType::CHTL_JS, source.substr(cjs_start, current - cjs_start));
                continue;

            } else {
                // It was a normal JS word, put it in the buffer
                buffer += word;
            }
        }

        if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;

        if (brace_level > 0) {
            buffer += advance();
        }
    }

    // Add any remaining JS content
    if (!buffer.empty()) {
        addFragment(FragmentType::JS, buffer);
    }

    // Consume the final '}'
    if (!isAtEnd() && peek() == '}') {
        advance();
    }

    state = ScannerState::SCANNING_CHTL; // Revert state
}

void CHTLUnifiedScanner::scanStyle() {
    size_t start = current;
    std::string buffer;
    int brace_level = 0;

    // Ensure we are at the start of the block content
    while (!isAtEnd() && peek() != '{') {
        advance();
    }
    if (isAtEnd()) return; // Should not happen with valid syntax
    advance(); // Consume '{'

    size_t content_start = current;
    int content_brace_level = 1;

    while (!isAtEnd() && content_brace_level > 0) {
        if (peek() == '@') {
            // Found a potential CHTL construct. Add preceding content as CSS.
            if (!buffer.empty()) {
                addFragment(FragmentType::CSS, buffer);
                buffer.clear();
            }

            // Extract the CHTL statement (e.g., "@Style ...;")
            size_t at_start = current;
            while (!isAtEnd() && peek() != ';') {
                advance();
            }
            advance(); // Consume ';'
            addFragment(FragmentType::CHTL, source.substr(at_start, current - at_start));
        } else {
            if (peek() == '{') content_brace_level++;
            else if (peek() == '}') content_brace_level--;

            if (content_brace_level > 0) { // Don't add the final '}' to the buffer
                buffer += advance();
            }
        }
    }

    // Add any remaining CSS content
    if (!buffer.empty()) {
        addFragment(FragmentType::CSS, buffer);
    }

    // Consume the final '}'
    if (!isAtEnd() && peek() == '}') {
        advance();
    }

    state = ScannerState::SCANNING_CHTL;
}


// Helper methods implementation
bool CHTLUnifiedScanner::isAtEnd() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) {
        if (source[current] == '\n') line++;
        current++;
    }
    return source[current - 1];
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLUnifiedScanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    advance(); advance(); // Consume /*
                    while(!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        advance();
                    }
                    advance(); advance(); // Consume */
                }
                else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    if (current + expected.length() > source.length()) {
        return false;
    }
    if (source.substr(current, expected.length()) == expected) {
        current += expected.length();
        return true;
    }
    return false;
}

void CHTLUnifiedScanner::addFragment(FragmentType type, const std::string& content) {
    if (!content.empty()) {
        fragments.push_back({type, content, line});
    }
}

bool CHTLUnifiedScanner::isChtlJsKeyword(const std::string& word) {
    return chtlJsKeywords.count(word);
}

} // namespace CHTL
