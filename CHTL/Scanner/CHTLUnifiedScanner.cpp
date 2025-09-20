#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

// This is a more robust implementation that correctly handles brace nesting.
std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int line_start = 1;
    int brace_level = 0;

    for (current = 0; current < source.length(); ++current) {
        // Naive comment skipping for this pass.
        if (source[current] == '/' && current + 1 < source.length()) {
            if (source[current + 1] == '/') {
                buffer += source[current];
                current++;
                while(current < source.length() && source[current] != '\n') {
                    buffer += source[current];
                    current++;
                }
                 if(current < source.length()) buffer += source[current];
                continue;
            }
            if (source[current + 1] == '*') {
                 buffer += source[current];
                 current++;
                 buffer += source[current];
                 current++;
                 while(current + 1 < source.length() && (source[current] != '*' || source[current+1] != '/')) {
                     buffer += source[current];
                     current++;
                 }
                 if(current < source.length()) buffer += source[current];
                 current++;
                 if(current < source.length()) buffer += source[current];
                 continue;
            }
        }

        // Look for top-level style/script blocks
        if (brace_level == 0) {
            bool is_style = source.substr(current, 5) == "style";
            bool is_script = source.substr(current, 6) == "script";

            // Lookahead to confirm it's a block and not just a property name
            int lookahead = current + (is_style ? 5 : (is_script ? 6 : 0));
            while(lookahead < source.length() && isspace(source[lookahead])) {
                lookahead++;
            }

            if ((is_style || is_script) && lookahead < source.length() && source[lookahead] == '{') {
                // Found a global block. Finalize the preceding CHTL fragment.
                if (!buffer.empty()) {
                    fragments.push_back({FragmentType::CHTL, buffer, line_start});
                    buffer.clear();
                }

                // Start of the new special fragment
                FragmentType type = is_style ? FragmentType::CSS : FragmentType::JS;
                int content_start = lookahead + 1;
                int content_line_start = line; // Approximate line

                current = content_start;
                int inner_brace_level = 1;
                while(current < source.length() && inner_brace_level > 0) {
                    if(source[current] == '{') inner_brace_level++;
                    if(source[current] == '}') inner_brace_level--;
                    current++;
                }

                int content_end = current - 1;
                fragments.push_back({
                    type,
                    source.substr(content_start, content_end - content_start),
                    content_line_start
                });

                // Reset for the next CHTL fragment
                current--; // The loop's ++ will put us at the char after '}'
                line_start = line;
                continue;
            }
        }

        if (source[current] == '{') brace_level++;
        if (source[current] == '}') brace_level--;

        buffer += source[current];
    }

    // Add the final buffer content as a CHTL fragment
    if (!buffer.empty()) {
        fragments.push_back({FragmentType::CHTL, buffer, line_start});
    }

    return fragments;
}

} // namespace CHTL
