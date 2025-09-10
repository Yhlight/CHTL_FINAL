#include "UnifiedScanner.h"
#include <stdexcept>

namespace CHTL {

std::vector<CodeFragment> UnifiedScanner::Scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    std::string current_chunk;

    size_t i = 0;
    while (i < source.length()) {
        // This is a more robust scanner that looks for `script` as a keyword
        // not followed by a colon, and then finds the block.
        // It's still not perfect as it doesn't handle comments or strings
        // that might contain the word "script". A truly robust scanner
        // would need a preliminary tokenization pass.

        size_t script_keyword_pos = source.find("script", i);

        // Find the next opening brace after the potential keyword
        size_t brace_pos = (script_keyword_pos != std::string::npos) ? source.find('{', script_keyword_pos) : std::string::npos;

        if (script_keyword_pos != std::string::npos && brace_pos != std::string::npos) {
            // Check if there's a colon between the keyword and the brace, which would mean it's a property.
            size_t colon_pos = source.find(':', script_keyword_pos);
            if (colon_pos > brace_pos) { // No colon, or colon is after the brace, so it's a block

                // Add the CHTL fragment before the script block
                current_chunk = source.substr(i, script_keyword_pos - i);
                if (!current_chunk.empty()) {
                    fragments.push_back({LanguageType::CHTL, current_chunk});
                }

                // Find the matching closing brace
                int brace_level = 1;
                size_t script_content_start = brace_pos + 1;
                size_t script_content_end = script_content_start;
                while (brace_level > 0 && script_content_end < source.length()) {
                    if (source[script_content_end] == '{') brace_level++;
                    else if (source[script_content_end] == '}') brace_level--;
                    script_content_end++;
                }

                if (brace_level == 0) {
                    std::string script_content = source.substr(script_content_start, script_content_end - script_content_start - 1);
                    fragments.push_back({LanguageType::CHTL_JS, script_content});
                    i = script_content_end;
                    continue;
                }
            }
        }

        // If no script block is found, the rest of the file is CHTL
        current_chunk = source.substr(i);
        if (!current_chunk.empty()) {
            fragments.push_back({LanguageType::CHTL, current_chunk});
        }
        break; // Exit the loop
    }

    return fragments;
}

} // namespace CHTL
