#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : m_source(source) {}

// This is a simplified, first-pass implementation of the scanner.
// It identifies top-level style and script blocks and separates them.
// It does NOT yet implement the complex placeholder logic for mixed CHTL JS and JS.
std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t last_pos = 0;
    m_cursor = 0;

    while (m_cursor < m_source.length()) {
        // Look for the next potential block keyword
        size_t style_keyword_pos = m_source.find("style", m_cursor);
        size_t script_keyword_pos = m_source.find("script", m_cursor);

        size_t next_block_pos = std::min(style_keyword_pos, script_keyword_pos);

        if (next_block_pos == std::string::npos) {
            // No more style or script blocks found
            break;
        }

        // Find the opening brace for the block
        size_t opening_brace = m_source.find('{', next_block_pos);
        if (opening_brace == std::string::npos) {
            // Keyword found but no block, move past it to avoid infinite loops
            m_cursor = next_block_pos + 1;
            continue;
        }

        // We need to ensure this is a top-level block, not a nested one.
        // This simple check works for blocks not nested in other braces.
        // A more robust implementation would need a proper parser.
        std::string preceding_text = m_source.substr(m_cursor, next_block_pos - m_cursor);
        if (preceding_text.find('{') != std::string::npos) {
             m_cursor = next_block_pos + 1;
             continue;
        }

        // Add the CHTL part before the block
        if (next_block_pos > last_pos) {
            fragments.push_back({m_source.substr(last_pos, next_block_pos - last_pos), FragmentType::CHTL});
        }

        // Add the block keyword itself as CHTL
        fragments.push_back({m_source.substr(next_block_pos, opening_brace - next_block_pos), FragmentType::CHTL});


        // Find the matching closing brace
        int brace_count = 1;
        size_t content_start = opening_brace + 1;
        size_t current_scan_pos = content_start;
        while (current_scan_pos < m_source.length() && brace_count > 0) {
            if (m_source[current_scan_pos] == '{') {
                brace_count++;
            } else if (m_source[current_scan_pos] == '}') {
                brace_count--;
            }
            current_scan_pos++;
        }

        if (brace_count == 0) {
            // Found the end of the block
            size_t content_end = current_scan_pos - 1;
            std::string content = m_source.substr(content_start, content_end - content_start);

            FragmentType type = (next_block_pos == script_keyword_pos) ? FragmentType::JS : FragmentType::CSS;
            fragments.push_back({content, type});

            // Add the closing brace as CHTL
            fragments.push_back({m_source.substr(content_end, 1), FragmentType::CHTL});

            last_pos = current_scan_pos;
            m_cursor = current_scan_pos;
        } else {
            // Unmatched brace, something is wrong. Treat the rest as CHTL and stop.
            m_cursor = m_source.length();
        }
    }

    // Add any remaining CHTL at the end of the file
    if (last_pos < m_source.length()) {
        fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
    }

    return fragments;
}

} // namespace CHTL
