#include "UnifiedScanner.h"
#include <regex>

std::vector<CodeSnippet> UnifiedScanner::scan(const std::string& source) {
    // For now, a simple implementation.
    // We'll find script blocks and style blocks.
    // Everything outside of them is considered CHTL.

    size_t last_pos = 0;
    std::regex block_regex(R"((style|script)\s*\{)");
    std::smatch match;

    auto current_search_start = source.cbegin();

    while (std::regex_search(current_search_start, source.cend(), match, block_regex)) {
        size_t block_start_pos = match.position(0) + (current_search_start - source.cbegin());

        // Add the content before this block as CHTL
        if (block_start_pos > last_pos) {
            snippets.push_back({CodeType::CHTL, source.substr(last_pos, block_start_pos - last_pos)});
        }

        // Find the matching closing brace for the block
        size_t brace_level = 1;
        size_t block_content_start = block_start_pos + match.length(0);
        size_t current_pos = block_content_start;
        while (brace_level > 0 && current_pos < source.length()) {
            if (source[current_pos] == '{') {
                brace_level++;
            } else if (source[current_pos] == '}') {
                brace_level--;
            }
            current_pos++;
        }

        if (brace_level != 0) {
            // Unmatched brace, handle error or just stop
            break;
        }

        size_t block_content_end = current_pos - 1;
        std::string block_content = source.substr(block_content_start, block_content_end - block_content_start);

        std::string block_type_str = match[1].str();
        CodeType block_type = (block_type_str == "script") ? CodeType::CHTL_JS : CodeType::CSS;

        // For now, we tag the whole block content.
        // The placeholder logic will be added later.
        snippets.push_back({block_type, block_content});

        last_pos = current_pos;
        current_search_start = source.cbegin() + last_pos;
    }

    // Add the remaining content as CHTL
    if (last_pos < source.length()) {
        snippets.push_back({CodeType::CHTL, source.substr(last_pos)});
    }

    return snippets;
}
