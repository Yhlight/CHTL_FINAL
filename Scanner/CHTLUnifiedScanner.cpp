#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {

// List of CHTL JS keywords that signify the start of a CHTL JS block
const std::vector<std::string> CHTLJS_KEYWORDS = {"Listen", "Delegate", "Animate", "Router", "Vir"};

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
    std::string chtl_js_stream;

    auto finalize_js_buffer = [&]() {
        if (!js_buffer.empty()) {
            std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(m_placeholder_counter++) + "__";
            m_fragments.push_back({js_buffer, FragmentType::JS, placeholder});
            chtl_js_stream += placeholder;
            js_buffer.clear();
        }
    };

    while (m_cursor < block_end) {
        bool is_chtl_js_construct = false;

        // CHTL JS constructs checks
        // ... ({{...}}, $...$, &->, keywords)

        size_t construct_end_pos = m_cursor;

        // Check for {{ ... }} and {{ ... }}->...
        if (m_source.substr(m_cursor, 2) == "{{") {
            size_t end_brace_pos = m_source.find("}}", m_cursor);
            if (end_brace_pos != std::string::npos) {
                construct_end_pos = end_brace_pos + 2;
                if (m_source.substr(construct_end_pos, 2) == "->") {
                    construct_end_pos += 2;
                    while (construct_end_pos < block_end && (std::isalnum(m_source[construct_end_pos]) || m_source[construct_end_pos] == '_')) {
                        construct_end_pos++;
                    }
                }
                is_chtl_js_construct = true;
            }
        }
        // Check for $...$
        else if (m_source[m_cursor] == '$') {
             size_t end_dollar_pos = m_source.find('$', m_cursor + 1);
             if (end_dollar_pos != std::string::npos) {
                 bool valid = true;
                 for(size_t i = m_cursor + 1; i < end_dollar_pos; ++i) {
                     if(std::isspace(m_source[i])) { valid = false; break; }
                 }
                 if(valid) {
                    construct_end_pos = end_dollar_pos + 1;
                    is_chtl_js_construct = true;
                 }
             }
        }
        // Check for &->
        else if (m_source.substr(m_cursor, 3) == "&->") {
            size_t end_pos = m_cursor + 3;
            while(end_pos < block_end && std::isspace(m_source[end_pos])) { end_pos++; }
            size_t end_of_line = m_source.find('\n', end_pos);
            size_t start_of_block = m_source.find('{', end_pos);
            if (start_of_block != std::string::npos && (start_of_block < end_of_line || end_of_line == std::string::npos)) {
                int brace_count = 1;
                size_t end_scan = start_of_block + 1;
                while(end_scan < block_end && brace_count > 0) {
                    if (m_source[end_scan] == '{') brace_count++;
                    else if (m_source[end_scan] == '}') brace_count--;
                    end_scan++;
                }
                if(brace_count == 0) construct_end_pos = end_scan;
            } else {
                construct_end_pos = end_of_line;
            }
            if (construct_end_pos == std::string::npos || construct_end_pos > block_end) {
                construct_end_pos = block_end;
            }
            is_chtl_js_construct = true;
        }
        else {
            for (const auto& keyword : CHTLJS_KEYWORDS) {
                if (m_source.substr(m_cursor, keyword.length()) == keyword) {
                    bool is_standalone = (m_cursor == 0 || !std::isalnum(m_source[m_cursor - 1]));
                    if (is_standalone && m_cursor + keyword.length() < m_source.length() && !std::isalnum(m_source[m_cursor + keyword.length()])) {
                        size_t search_pos = m_cursor + keyword.length();
                        while(search_pos < block_end && std::isspace(m_source[search_pos])) { search_pos++; }
                        if (search_pos < block_end && m_source[search_pos] == '{') {
                            int brace_count = 1;
                            size_t end_scan = search_pos + 1;
                            while(end_scan < block_end && brace_count > 0) {
                                if (m_source[end_scan] == '{') brace_count++;
                                else if (m_source[end_scan] == '}') brace_count--;
                                end_scan++;
                            }
                            if (brace_count == 0) {
                                construct_end_pos = end_scan;
                                is_chtl_js_construct = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (is_chtl_js_construct) {
            finalize_js_buffer();
            chtl_js_stream += m_source.substr(m_cursor, construct_end_pos - m_cursor);
            m_cursor = construct_end_pos;
        } else {
            js_buffer += m_source[m_cursor];
            m_cursor++;
        }
    }

    finalize_js_buffer();

    if (!chtl_js_stream.empty()) {
        m_fragments.push_back({chtl_js_stream, FragmentType::CHTL_JS, ""});
    }
}

} // namespace CHTL
