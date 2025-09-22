#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {

// List of CHTL JS keywords that signify the start of a CHTL JS block
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "Listen", "Delegate", "Animate", "Router", "Vir",
    "printMylove", "iNeverAway", "util", "ScriptLoader"
};

// CHTL features that can appear in a global style block
const std::vector<std::string> CHTL_STYLE_FEATURES = {
    "ThemeColor", "@Style", "delete", "inherit"
    // Note: Property arithmetic (e.g. `width: 100px + 50px`) and
    // conditional expressions are harder to detect with simple string searching
    // and will require a more advanced parsing approach in the next step.
};

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

    // Look for the next special block
    size_t script_pos = m_source.find("script", m_cursor);
    size_t style_pos = m_source.find("style", m_cursor);

    size_t next_block_pos = std::min(script_pos, style_pos);

    if (next_block_pos == std::string::npos) {
        // No more special blocks, the rest is CHTL
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length(); // End the loop
        return;
    }

    // Found a block. Find its opening brace.
    size_t opening_brace = m_source.find('{', next_block_pos);
    if (opening_brace == std::string::npos) {
        m_cursor = next_block_pos + 1; // Move past the keyword to avoid infinite loops
        return;
    }

    // Add the CHTL part before the block
    if (opening_brace > last_pos) {
         m_fragments.push_back({m_source.substr(last_pos, opening_brace + 1 - last_pos), FragmentType::CHTL});
    } else {
        // The block starts right away, push the keyword and brace
         m_fragments.push_back({m_source.substr(next_block_pos, opening_brace + 1 - next_block_pos), FragmentType::CHTL});
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

        if (next_block_pos == script_pos) {
            scanScriptContent(block_end);
        } else if (next_block_pos == style_pos) {
            scanStyleContent(block_end);
        }

        m_fragments.push_back({m_source.substr(block_end, 1), FragmentType::CHTL}); // Add the closing brace
        m_cursor = block_end + 1;
    } else {
        // Unmatched brace, treat the rest as CHTL
        m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        m_cursor = m_source.length();
    }
}

void CHTLUnifiedScanner::scanStyleContent(size_t block_end) {
    std::string css_buffer;

    auto finalize_css_buffer = [&]() {
        if (!css_buffer.empty()) {
            m_fragments.push_back({css_buffer, FragmentType::CSS});
            css_buffer.clear();
        }
    };

    while (m_cursor < block_end) {
        size_t next_feature_pos = std::string::npos;

        // Look for CHTL-specific keywords
        for (const auto& feature : CHTL_STYLE_FEATURES) {
            size_t feature_pos = m_source.find(feature, m_cursor);
            if (feature_pos != std::string::npos && feature_pos < block_end) {
                next_feature_pos = std::min(next_feature_pos, feature_pos);
            }
        }

        // A simple check for property arithmetic and conditionals
        size_t next_operator_pos = m_source.find_first_of("+-*/?@", m_cursor);
        if (next_operator_pos != std::string::npos && next_operator_pos < block_end) {
             // More checks to avoid flagging CSS calc() or other valid uses
            if (m_source[next_operator_pos] == '@' || (m_source.find("calc(", next_operator_pos - 5, 5) == std::string::npos)) {
                 next_feature_pos = std::min(next_feature_pos, next_operator_pos);
            }
        }

        if (next_feature_pos == std::string::npos || next_feature_pos >= block_end) {
            // No more CHTL features, the rest is CSS
            css_buffer += m_source.substr(m_cursor, block_end - m_cursor);
            m_cursor = block_end;
            break;
        }

        // The part before the feature is CSS
        if (next_feature_pos > m_cursor) {
            css_buffer += m_source.substr(m_cursor, next_feature_pos - m_cursor);
        }

        finalize_css_buffer();

        // Process the CHTL feature
        m_cursor = next_feature_pos;
        size_t feature_start = m_cursor;

        // Simple forward scanning for the end of the statement
        size_t end_pos = m_source.find_first_of(";", m_cursor);
        if (end_pos == std::string::npos || end_pos > block_end) {
            end_pos = block_end;
        } else {
            end_pos++; // include the semicolon
        }

        m_fragments.push_back({m_source.substr(feature_start, end_pos - feature_start), FragmentType::CHTL});
        m_cursor = end_pos;
    }

    finalize_css_buffer(); // Add any remaining CSS
}

void CHTLUnifiedScanner::scanScriptContent(size_t block_end) {
    // This implementation is still basic and will be improved.
    // It looks for a few CHTL JS constructs and separates them from JS.
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
        // Find the start of a potential CHTL JS construct
        size_t next_open_brace = m_source.find("{{", m_cursor);
        size_t next_dollar_sign = m_source.find("$", m_cursor);

        size_t next_construct_pos = std::min(next_open_brace, next_dollar_sign);

        // Check for CHTL JS keywords
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t keyword_pos = m_source.find(keyword, m_cursor);
            if (keyword_pos != std::string::npos) {
                 next_construct_pos = std::min(next_construct_pos, keyword_pos);
            }
        }

        if (next_construct_pos == std::string::npos || next_construct_pos >= block_end) {
            // No more CHTL JS constructs in this block, the rest is JS
            js_buffer += m_source.substr(m_cursor, block_end - m_cursor);
            m_cursor = block_end;
            break;
        }

        // The part before the construct is JS
        if (next_construct_pos > m_cursor) {
            js_buffer += m_source.substr(m_cursor, next_construct_pos - m_cursor);
        }

        finalize_js_buffer();

        // Process the CHTL JS construct
        m_cursor = next_construct_pos;
        size_t construct_start = m_cursor;

        // Simple forward scanning for the end of the construct. This is naive and needs improvement.
        size_t end_pos = m_source.find_first_of("};),", m_cursor);
        if (end_pos == std::string::npos || end_pos > block_end) {
            end_pos = block_end;
        }

        // If it's a block construct, find the matching brace
        size_t open_brace = m_source.find('{', m_cursor);
        if (open_brace != std::string::npos && open_brace < end_pos) {
             int brace_count = 1;
             size_t scan_pos = open_brace + 1;
             while (scan_pos < block_end && brace_count > 0) {
                 if (m_source[scan_pos] == '{') brace_count++;
                 else if (m_source[scan_pos] == '}') brace_count--;
                 scan_pos++;
             }
             if (brace_count == 0) {
                 end_pos = scan_pos;
             }
        } else {
            end_pos += 1; // include the terminator
        }


        chtl_js_buffer += m_source.substr(construct_start, end_pos - construct_start);
        m_cursor = end_pos;
    }

    finalize_js_buffer(); // Add any remaining JS
    if (!chtl_js_buffer.empty()) {
        m_fragments.push_back({chtl_js_buffer, FragmentType::CHTL_JS});
    }
}

} // namespace CHTL
