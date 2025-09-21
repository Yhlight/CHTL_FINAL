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
    std::string js_buffer;
    std::string chtl_js_buffer;

    auto finalize_js_buffer = [&]() {
        if (!js_buffer.empty()) {
            std::string placeholder_id = "_JS_PLACEHOLDER_" + std::to_string(m_placeholder_counter++) + "_";
            m_fragments.push_back({js_buffer, FragmentType::JS, placeholder_id});
            chtl_js_buffer += placeholder_id;
            js_buffer.clear();
        }
    };

    while (m_cursor < block_end) {
        size_t start_pos = m_cursor;

        // Find the start of a potential CHTL JS construct
        size_t next_open_brace = m_source.find("{{", m_cursor);
        size_t next_arrow = m_source.find("->", m_cursor);
        size_t next_amp_arrow = m_source.find("&->", m_cursor);

        size_t next_construct_pos = std::string::npos;
        if (next_open_brace != std::string::npos) next_construct_pos = std::min(next_construct_pos, next_open_brace);
        if (next_arrow != std::string::npos) next_construct_pos = std::min(next_construct_pos, next_arrow);
        if (next_amp_arrow != std::string::npos) next_construct_pos = std::min(next_construct_pos, next_amp_arrow);

        if (next_construct_pos == std::string::npos) {
            // No more CHTL JS constructs in this block, the rest is JS
            js_buffer += m_source.substr(m_cursor, block_end - m_cursor);
            m_cursor = block_end;
            break;
        }

        // The part before the construct is JS
        if (next_construct_pos > m_cursor) {
            js_buffer += m_source.substr(m_cursor, next_construct_pos - m_cursor);
        }

        // Finalize the JS buffer before processing the CHTL JS construct
        finalize_js_buffer();

        // Process the CHTL JS construct
        m_cursor = next_construct_pos;
        size_t construct_start = m_cursor;

        if (m_source.substr(m_cursor, 2) == "{{") {
            size_t end_pos = m_source.find("}}", m_cursor);
            if (end_pos != std::string::npos) {
                m_cursor = end_pos + 2;
            }
        } else if (m_source.substr(m_cursor, 3) == "&->") {
            m_cursor += 3;
        } else if (m_source.substr(m_cursor, 2) == "->") {
            m_cursor += 2;
        }

        // After a prefix, check for a keyword and its block
        while(m_cursor < block_end && std::isspace(m_source[m_cursor])) m_cursor++;

        bool keyword_found = false;
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            if (m_source.substr(m_cursor, keyword.length()) == keyword) {
                m_cursor += keyword.length();
                while(m_cursor < block_end && std::isspace(m_source[m_cursor])) m_cursor++;
                if (m_cursor < block_end && m_source[m_cursor] == '{') {
                    int brace_count = 1;
                    m_cursor++;
                    while(m_cursor < block_end && brace_count > 0) {
                        if (m_source[m_cursor] == '{') brace_count++;
                        else if (m_source[m_cursor] == '}') brace_count--;
                        m_cursor++;
                    }
                }
                keyword_found = true;
                break;
            }
        }

        chtl_js_buffer += m_source.substr(construct_start, m_cursor - construct_start);
    }

    finalize_js_buffer(); // Add any remaining JS
    if (!chtl_js_buffer.empty()) {
        m_fragments.push_back({chtl_js_buffer, FragmentType::CHTL_JS});
    }
}

} // namespace CHTL
