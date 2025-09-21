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

void CHTLUnifiedScanner::scanStyleContent(size_t block_end) {
    std::string css_buffer;

    auto finalize_css_buffer = [&]() {
        if (!css_buffer.empty()) {
            m_fragments.push_back({css_buffer, FragmentType::CSS});
            css_buffer.clear();
        }
    };

    size_t current_pos = m_cursor;
    while (current_pos < block_end) {
        // Find the next statement separator. In CSS, this is typically ';' or '}'.
        size_t end_of_statement = m_source.find(';', current_pos);
        size_t end_of_block = m_source.find('}', current_pos);

        // Determine the actual end of the current statement.
        size_t end_pos = block_end; // Default to the end of the whole block.
        if (end_of_statement != std::string::npos && end_of_statement < end_pos) {
            end_pos = end_of_statement;
        }
        if (end_of_block != std::string::npos && end_of_block < end_pos) {
            end_pos = end_of_block;
        }

        // Include the separator (';' or '}') in the statement itself.
        if (end_pos < block_end) {
            end_pos++;
        }

        std::string statement = m_source.substr(current_pos, end_pos - current_pos);

        // Use heuristics to detect CHTL-specific features in the statement.
        bool is_chtl = (
            statement.find('@') != std::string::npos ||      // For @Style, @Var templates
            statement.find('[') != std::string::npos ||      // For [Origin], [Custom], etc.
            statement.find('#') != std::string::npos ||      // For # generator comments
            statement.find('?') != std::string::npos ||      // For conditional properties
            statement.find('$') != std::string::npos ||      // For $variable$ reactivity
            // Check for '+' or '*' outside of a 'calc()' to detect simple arithmetic.
            // This is a basic check and might have edge cases.
            (statement.find('+') != std::string::npos && statement.rfind("calc", statement.find('+')) == std::string::npos) ||
            (statement.find('*') != std::string::npos && statement.rfind("calc", statement.find('*')) == std::string::npos)
        );

        if (is_chtl) {
            // If a CHTL feature is detected, finalize any pending CSS buffer
            // and create a distinct CHTL fragment for this statement.
            finalize_css_buffer();
            m_fragments.push_back({statement, FragmentType::CHTL});
        } else {
            // If no CHTL features are found, assume it's standard CSS and append it to the buffer.
            css_buffer += statement;
        }

        current_pos = end_pos;
    }

    // After the loop, finalize any remaining CSS in the buffer.
    finalize_css_buffer();
    m_cursor = block_end; // Ensure the main scanner cursor is advanced past this block.
}

void CHTLUnifiedScanner::scanSource() {
    size_t last_pos = m_cursor;

    // Find the next occurrence of "style" or "script"
    size_t style_pos = m_source.find("style", m_cursor);
    size_t script_pos = m_source.find("script", m_cursor);

    size_t next_keyword_pos = std::string::npos;
    std::string block_type;

    // Determine which block type comes first
    if (style_pos != std::string::npos && (script_pos == std::string::npos || style_pos < script_pos)) {
        next_keyword_pos = style_pos;
        block_type = "style";
    } else if (script_pos != std::string::npos) {
        next_keyword_pos = script_pos;
        block_type = "script";
    }

    if (next_keyword_pos == std::string::npos) {
        // No more style or script blocks found, the rest of the source is CHTL
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length(); // End the loop
        return;
    }

    // Found a keyword. Now, find its opening brace.
    size_t opening_brace = m_source.find('{', next_keyword_pos);
    if (opening_brace == std::string::npos) {
        // This is likely not a block definition, or the code is malformed.
        // Skip past the found keyword to avoid an infinite loop and retry scanning.
        m_cursor = next_keyword_pos + 1;
        return;
    }

    // The content from the last position up to and including the opening brace is CHTL
    if (opening_brace >= last_pos) {
        m_fragments.push_back({m_source.substr(last_pos, opening_brace + 1 - last_pos), FragmentType::CHTL});
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

        if (block_type == "script") {
            scanScriptContent(block_end);
        } else if (block_type == "style") {
            scanStyleContent(block_end);
        }

        // The closing brace is part of the CHTL structure
        m_fragments.push_back({m_source.substr(block_end, 1), FragmentType::CHTL});
        m_cursor = block_end + 1;
    } else {
        // Unmatched brace, something is wrong. Treat the rest of the file as CHTL and finish.
        m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        m_cursor = m_source.length();
    }
}

void CHTLUnifiedScanner::scanScriptAndSubscript(size_t block_end, std::string& chtl_js_buffer) {
    std::string js_buffer;

    auto finalize_js_buffer = [&]() {
        if (!js_buffer.empty()) {
            std::string placeholder_id = "_JS_PLACEHOLDER_" + std::to_string(m_placeholder_counter++) + "_";
            m_fragments.push_back({js_buffer, FragmentType::JS, placeholder_id});
            chtl_js_buffer += placeholder_id;
            js_buffer.clear();
        }
    };

    while (m_cursor < block_end) {
        size_t nearest_pos = block_end;
        std::string found_construct = "";

        size_t brace_pos = m_source.find("{{", m_cursor);
        if (brace_pos != std::string::npos && brace_pos < nearest_pos) {
            nearest_pos = brace_pos;
            found_construct = "{{";
        }

        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t keyword_pos = m_source.find(keyword, m_cursor);
            if (keyword_pos != std::string::npos && keyword_pos < nearest_pos) {
                bool is_standalone = (keyword_pos == 0 || !std::isalnum(m_source[keyword_pos - 1]));
                 if (is_standalone && (keyword_pos + keyword.length() >= m_source.length() || !std::isalnum(m_source[keyword_pos + keyword.length()]))) {
                    nearest_pos = keyword_pos;
                    found_construct = keyword;
                }
            }
        }

        js_buffer += m_source.substr(m_cursor, nearest_pos - m_cursor);

        if (nearest_pos == block_end) {
            m_cursor = block_end;
            break;
        }

        finalize_js_buffer();

        m_cursor = nearest_pos;

        if (found_construct == "{{") {
            size_t end_brace_pos = m_source.find("}}", m_cursor);
            if (end_brace_pos != std::string::npos && end_brace_pos < block_end) {
                chtl_js_buffer += m_source.substr(m_cursor, end_brace_pos + 2 - m_cursor);
                m_cursor = end_brace_pos + 2;
            } else { // Invalid construct
                chtl_js_buffer += m_source.substr(m_cursor, 2);
                m_cursor += 2;
            }
        } else { // It's a keyword
            size_t open_brace_pos = m_source.find('{', m_cursor + found_construct.length());
            if (open_brace_pos != std::string::npos && open_brace_pos < block_end) {
                chtl_js_buffer += m_source.substr(m_cursor, open_brace_pos + 1 - m_cursor);
                m_cursor = open_brace_pos + 1;

                int brace_count = 1;
                size_t scan_pos = m_cursor;
                while (scan_pos < block_end && brace_count > 0) {
                    if (m_source[scan_pos] == '{') brace_count++;
                    else if (m_source[scan_pos] == '}') brace_count--;
                    scan_pos++;
                }

                if (brace_count == 0) {
                    size_t sub_block_end = scan_pos - 1;
                    scanScriptAndSubscript(sub_block_end, chtl_js_buffer); // Recursive call
                    chtl_js_buffer += '}';
                    m_cursor = sub_block_end + 1;
                } else { // Unmatched brace
                    // Consider the rest of the block as part of the CHTL JS construct
                }
            } else { // Not a block, treat as JS
                js_buffer += found_construct;
                m_cursor += found_construct.length();
            }
        }
    }
    finalize_js_buffer();
}


void CHTLUnifiedScanner::scanScriptContent(size_t block_end) {
    std::string chtl_js_fragment_content;
    scanScriptAndSubscript(block_end, chtl_js_fragment_content);

    if (!chtl_js_fragment_content.empty()) {
        m_fragments.push_back({chtl_js_fragment_content, FragmentType::CHTL_JS});
    }
}

} // namespace CHTL
