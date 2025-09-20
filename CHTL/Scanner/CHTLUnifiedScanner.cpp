#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

bool CHTLUnifiedScanner::isAtEnd() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    current++;
    return source[current - 1];
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void CHTLUnifiedScanner::skipWhitespaceAndComments() {
    while (true) {
        if (isAtEnd()) break;
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
                if (current + 1 < source.length() && source[current + 1] == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (current + 1 < source.length() && source[current + 1] == '*') {
                    advance(); // consume /
                    advance(); // consume *
                    while (current + 1 < source.length() && !(source[current] == '*' && source[current+1] == '/')) {
                        if(peek() == '\n') line++;
                        advance();
                    }
                    if(!isAtEnd()) advance(); // consume *
                    if(!isAtEnd()) advance(); // consume /
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


std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t last_fragment_end = 0;
    int last_fragment_line = 1;

    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        size_t block_start = current;

        // Check for global style or script blocks
        bool is_style = source.substr(current, 5) == "style";
        bool is_script = source.substr(current, 6) == "script";

        size_t keyword_len = is_style ? 5 : (is_script ? 6 : 0);

        if (is_style || is_script) {
            size_t potential_brace = current + keyword_len;
            while(potential_brace < source.length() && isspace(source[potential_brace])) {
                potential_brace++;
            }

            if (potential_brace < source.length() && source[potential_brace] == '{') {
                // We found a block. First, let's commit the preceding CHTL fragment.
                if (block_start > last_fragment_end) {
                    fragments.push_back({
                        FragmentType::CHTL,
                        source.substr(last_fragment_end, block_start - last_fragment_end),
                        last_fragment_line
                    });
                }

                // Now, extract the content of the block.
                current = potential_brace + 1; // move past '{'
                int content_line_start = line;
                int brace_level = 1;
                size_t content_start = current;

                while(!isAtEnd() && brace_level > 0) {
                    if (peek() == '{') brace_level++;
                    else if (peek() == '}') brace_level--;
                    else if (peek() == '\n') line++;
                    advance();
                }

                if (brace_level == 0) {
                    size_t content_end = current - 1;
                    fragments.push_back({
                        is_style ? FragmentType::CSS : FragmentType::JS,
                        source.substr(content_start, content_end - content_start),
                        content_line_start
                    });
                    last_fragment_end = current;
                    last_fragment_line = line;
                } else {
                    // This case means unclosed braces, but we'll let the parser handle it.
                    // For now, we treat it as CHTL.
                    current = block_start; // backtrack
                    advance(); // ensure progress
                }
                continue;
            }
        }
        advance(); // If not a special block, just move on.
    }

    // Add the final trailing CHTL fragment if any
    if (last_fragment_end < source.length()) {
         fragments.push_back({
            FragmentType::CHTL,
            source.substr(last_fragment_end, source.length() - last_fragment_end),
            last_fragment_line
        });
    }

    return fragments;
}

} // namespace CHTL
