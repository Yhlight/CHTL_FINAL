#include "CHTLUnifiedScanner.h"
#include "CHTL/Config/Configuration.h"
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source, std::shared_ptr<Configuration> config)
    : m_source(source), m_config(config) {}

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

    // Look for the next script or style block
    size_t script_pos = m_source.find("script", m_cursor);
    size_t style_pos = m_source.find("style", m_cursor);

    size_t keyword_pos = std::string::npos;
    bool is_script = false;

    if (script_pos != std::string::npos && style_pos != std::string::npos) {
        keyword_pos = std::min(script_pos, style_pos);
        is_script = (keyword_pos == script_pos);
    } else if (script_pos != std::string::npos) {
        keyword_pos = script_pos;
        is_script = true;
    } else {
        keyword_pos = style_pos;
        is_script = false;
    }

    if (keyword_pos == std::string::npos) {
        // No more script or style blocks, the rest is CHTL
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length(); // End the loop
        return;
    }

    // Found a keyword. Find its block.
    size_t opening_brace = m_source.find('{', keyword_pos);
    if (opening_brace == std::string::npos) {
        // Malformed, skip keyword
        m_cursor = keyword_pos + 1;
        return;
    }

    // Add the CHTL part before the block
    if (opening_brace > last_pos) {
         m_fragments.push_back({m_source.substr(last_pos, opening_brace + 1 - last_pos), FragmentType::CHTL});
    } else {
        // Handle case where block starts immediately
        m_fragments.push_back({m_source.substr(last_pos, opening_brace - last_pos), FragmentType::CHTL});
        m_fragments.push_back({"{", FragmentType::CHTL});
    }


    // Find the matching closing brace for the block
    int brace_count = 1;
    size_t block_scan_pos = opening_brace + 1;
    while (block_scan_pos < m_source.length() && brace_count > 0) {
        if (m_source[block_scan_pos] == '{') brace_count++;
        else if (m_source[block_scan_pos] == '}') brace_count--;
        block_scan_pos++;
    }

    if (brace_count == 0) {
        // We found the end of the block.
        size_t block_end = block_scan_pos - 1;
        m_cursor = opening_brace + 1;

        if (is_script) {
            scanScriptContent(block_end);
        } else {
            scanStyleContent(block_end);
        }

        m_fragments.push_back({m_source.substr(block_end, 1), FragmentType::CHTL}); // Add the closing brace
        m_cursor = block_end + 1;
    } else {
        // Unmatched brace, treat from last_pos to end as CHTL
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
        bool consumed = false;

        // 1. Check for {{...}}
        if (m_source.substr(m_cursor, 2) == "{{") {
            finalize_js_buffer();
            size_t end_pos = m_source.find("}}", m_cursor);
            if (end_pos != std::string::npos && end_pos < block_end) {
                chtl_js_buffer += m_source.substr(m_cursor, end_pos + 2 - m_cursor);
                m_cursor = end_pos + 2;
                consumed = true;
            }
        }
        // 2. Check for &-> operator
        else if (m_source.substr(m_cursor, 3) == "&->") {
            finalize_js_buffer();
            chtl_js_buffer += "&->";
            m_cursor += 3;
            consumed = true;
        }
        // NEW: Check for -> operator
        else if (m_source.substr(m_cursor, 2) == "->") {
            finalize_js_buffer();
            chtl_js_buffer += "->";
            m_cursor += 2;
            consumed = true;
        }
        // 3. Check for keywords
        else {
            for (const auto& keyword : m_config->getCHTLJSKeywords()) {
                if (m_source.substr(m_cursor, keyword.length()) == keyword) {
                    bool is_standalone = (m_cursor == 0 || !std::isalnum(m_source[m_cursor - 1]));
                    if (is_standalone && (m_cursor + keyword.length() >= m_source.length() || !std::isalnum(m_source[m_cursor + keyword.length()]))) {
                        finalize_js_buffer();

                        // Special handling for 'Vir identifier ='
                        if (keyword == "Vir") {
                             size_t after_vir = m_cursor + keyword.length();
                             while(after_vir < block_end && std::isspace(m_source[after_vir])) after_vir++;
                             size_t identifier_end = after_vir;
                             while(identifier_end < block_end && std::isalnum(m_source[identifier_end])) identifier_end++;
                             size_t after_identifier = identifier_end;
                             while(after_identifier < block_end && std::isspace(m_source[after_identifier])) after_identifier++;
                             if(after_identifier < block_end && m_source[after_identifier] == '=') {
                                 chtl_js_buffer += m_source.substr(m_cursor, after_identifier + 1 - m_cursor);
                                 m_cursor = after_identifier + 1;
                                 consumed = true;
                                 break;
                             }
                        }

                        // Standard 'keyword {...}' block
                        size_t search_pos = m_cursor + keyword.length();
                        while(search_pos < block_end && std::isspace(m_source[search_pos])) search_pos++;
                        if (search_pos < block_end && m_source[search_pos] == '{') {
                            // Found a CHTL-JS block, now find its end
                            int brace_count = 1;
                            size_t sub_block_scan_pos = search_pos + 1;
                            while(sub_block_scan_pos < block_end && brace_count > 0) {
                                if (m_source[sub_block_scan_pos] == '{') brace_count++;
                                else if (m_source[sub_block_scan_pos] == '}') brace_count--;
                                sub_block_scan_pos++;
                            }

                            if (brace_count == 0) {
                                size_t sub_block_end = sub_block_scan_pos - 1;
                                // Add the keyword and opening brace
                                chtl_js_buffer += m_source.substr(m_cursor, search_pos + 1 - m_cursor);
                                m_cursor = search_pos + 1;
                                // Recursively scan the content of the sub-block
                                scanScriptContent(sub_block_end);
                                // Add the closing brace
                                chtl_js_buffer += m_source.substr(sub_block_end, 1);
                                m_cursor = sub_block_end + 1;
                                consumed = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (!consumed) {
            js_buffer += m_source[m_cursor];
            m_cursor++;
        }
    }

    finalize_js_buffer(); // Add any remaining JS
    if (!chtl_js_buffer.empty()) {
        m_fragments.push_back({chtl_js_buffer, FragmentType::CHTL_JS});
    }
}

void CHTLUnifiedScanner::scanStyleContent(size_t block_end) {
    // For now, treat the entire content of a style block as CSS.
    // The logic to separate CHTL features from CSS will be added later.
    if (m_cursor < block_end) {
        std::string content = m_source.substr(m_cursor, block_end - m_cursor);
        if (!content.empty()) {
            m_fragments.push_back({content, FragmentType::CSS});
        }
        m_cursor = block_end;
    }
}

} // namespace CHTL
