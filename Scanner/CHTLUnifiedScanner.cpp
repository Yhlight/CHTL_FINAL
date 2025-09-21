#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {

// List of CHTL JS keywords that signify the start of a CHTL JS block
const std::vector<std::string> CHTLJS_KEYWORDS = {"Listen", "Delegate", "Animate", "Router", "Vir", "printMylove", "iNeverAway"};

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : m_source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    m_fragments.clear();
    m_cursor = 0;
    m_placeholder_counter = 0;

    while (m_cursor < m_source.length()) {
        scanSource();
    }

    return m_fragments;
}

void CHTLUnifiedScanner::scanSource() {
    size_t last_pos = m_cursor;

    // Look for the next script block
    size_t script_keyword_pos = m_source.find("script", m_cursor);

    if (script_keyword_pos == std::string::npos) {
        // No more script blocks, the rest is CHTL
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length(); // End the loop
        return;
    }

    // Found a script keyword. Find its block.
    size_t opening_brace = m_source.find('{', script_keyword_pos);
    if (opening_brace == std::string::npos) {
        m_cursor = script_keyword_pos + 1;
        return;
    }

    // Add the CHTL part before the script block
    if (opening_brace > last_pos) {
        m_fragments.push_back({m_source.substr(last_pos, opening_brace + 1 - last_pos), FragmentType::CHTL});
    }

    // Find the matching closing brace for the script block
    int brace_count = 1;
    size_t block_scan_pos = opening_brace + 1;
    while (block_scan_pos < m_source.length() && brace_count > 0) {
        if (m_source[block_scan_pos] == '{') brace_count++;
        else if (m_source[block_scan_pos] == '}') brace_count--;
        block_scan_pos++;
    }

    if (brace_count == 0) {
        // We found the end of the script block.
        size_t block_end = block_scan_pos -1;
        m_cursor = opening_brace + 1;
        scanScriptContent(block_end); // Scan the content for mixed JS and CHTL JS
        m_fragments.push_back({m_source.substr(block_end, 1), FragmentType::CHTL}); // Add the closing brace
        m_cursor = block_end + 1;
    } else {
        // Unmatched brace, treat the rest as CHTL
        m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        m_cursor = m_source.length();
    }
}

void CHTLUnifiedScanner::scanScriptContent(size_t block_end) {
    size_t start_of_script = m_cursor;

    while (m_cursor < block_end) {
        // Find the next CHTL JS construct
        size_t next_mustache = m_source.find("{{", m_cursor);

        // Find the closest keyword
        size_t next_keyword = std::string::npos;
        const std::string* found_keyword = nullptr;
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t pos = m_source.find(keyword, m_cursor);
            if (pos != std::string::npos) {
                bool is_standalone = (pos == start_of_script || !std::isalnum(m_source[pos - 1]));
                if (is_standalone && (pos + keyword.length() >= block_end || !std::isalnum(m_source[pos + keyword.length()]))) {
                    if (next_keyword == std::string::npos || pos < next_keyword) {
                        next_keyword = pos;
                        found_keyword = &keyword;
                    }
                }
            }
        }

        size_t next_construct_pos = (next_mustache < next_keyword) ? next_mustache : next_keyword;

        if (next_construct_pos == std::string::npos || next_construct_pos >= block_end) {
            // No more CHTL JS constructs, the rest is JS
            if (m_cursor < block_end) {
                m_fragments.push_back({m_source.substr(m_cursor, block_end - m_cursor), FragmentType::JS});
            }
            break; // Exit loop
        }

        // Add the pure JS part before the construct
        if (next_construct_pos > m_cursor) {
            m_fragments.push_back({m_source.substr(m_cursor, next_construct_pos - m_cursor), FragmentType::JS});
        }

        m_cursor = next_construct_pos;

        // Handle the CHTL JS construct
        if (m_cursor == next_mustache) {
            size_t mustache_end = m_source.find("}}", m_cursor);
            if (mustache_end != std::string::npos && mustache_end < block_end) {
                size_t after_mustache = mustache_end + 2;

                std::string listen_arrow = "->Listen";
                size_t listen_arrow_pos = after_mustache;
                while(listen_arrow_pos < block_end && std::isspace(m_source[listen_arrow_pos])) listen_arrow_pos++;

                if (listen_arrow_pos < block_end && m_source.substr(listen_arrow_pos, listen_arrow.length()) == listen_arrow) {
                    size_t listen_brace_pos = listen_arrow_pos + listen_arrow.length();
                    while(listen_brace_pos < block_end && std::isspace(m_source[listen_brace_pos])) listen_brace_pos++;

                    if (listen_brace_pos < block_end && m_source[listen_brace_pos] == '{') {
                        int brace_count = 1;
                        size_t end_scan = listen_brace_pos + 1;
                        while (end_scan < block_end && brace_count > 0) {
                            if (m_source[end_scan] == '{') brace_count++;
                            else if (m_source[end_scan] == '}') brace_count--;
                            end_scan++;
                        }
                        if (brace_count == 0) {
                            m_fragments.push_back({m_source.substr(m_cursor, end_scan - m_cursor), FragmentType::CHTL_JS});
                            m_cursor = end_scan;
                            continue;
                        }
                    }
                }
                m_fragments.push_back({m_source.substr(m_cursor, after_mustache - m_cursor), FragmentType::CHTL_JS});
                m_cursor = after_mustache;
            } else {
                 m_cursor = next_mustache + 2; // Skip unterminated
            }
        } else { // It's a keyword
            size_t opening_brace = m_source.find('{', m_cursor);
            if (opening_brace != std::string::npos && opening_brace < block_end) {
                int brace_count = 1;
                size_t end_scan = opening_brace + 1;
                while (end_scan < block_end && brace_count > 0) {
                    if (m_source[end_scan] == '{') brace_count++;
                    else if (m_source[end_scan] == '}') brace_count--;
                    end_scan++;
                }
                if (brace_count == 0) {
                    m_fragments.push_back({m_source.substr(m_cursor, end_scan - m_cursor), FragmentType::CHTL_JS});
                    m_cursor = end_scan;
                } else {
                    m_cursor += found_keyword->length(); // Skip keyword
                }
            } else {
                m_cursor += found_keyword->length(); // Skip keyword
            }
        }
    }
}

} // namespace CHTL
