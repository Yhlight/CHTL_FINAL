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

    // Find the next occurrence of "script" or "style"
    size_t script_pos = m_source.find("script", m_cursor);
    size_t style_pos = m_source.find("style", m_cursor);

    size_t next_block_keyword_pos = std::string::npos;
    std::string block_type;

    // Determine which block type comes first
    if (script_pos != std::string::npos && style_pos != std::string::npos) {
        if (script_pos < style_pos) {
            next_block_keyword_pos = script_pos;
            block_type = "script";
        } else {
            next_block_keyword_pos = style_pos;
            block_type = "style";
        }
    } else if (script_pos != std::string::npos) {
        next_block_keyword_pos = script_pos;
        block_type = "script";
    } else if (style_pos != std::string::npos) {
        next_block_keyword_pos = style_pos;
        block_type = "style";
    }

    // If no more blocks are found, the rest of the file is CHTL
    if (next_block_keyword_pos == std::string::npos) {
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length();
        return;
    }

    // Add the CHTL content that comes before the block keyword
    if (next_block_keyword_pos > last_pos) {
        m_fragments.push_back({m_source.substr(last_pos, next_block_keyword_pos - last_pos), FragmentType::CHTL});
    }

    // Find the opening brace for the block
    size_t opening_brace = m_source.find('{', next_block_keyword_pos);
    if (opening_brace == std::string::npos) {
        // Malformed block, treat as CHTL and continue
        m_cursor = next_block_keyword_pos + block_type.length();
        return;
    }

    // Find the matching closing brace
    int brace_count = 1;
    size_t block_scan_pos = opening_brace + 1;
    while (block_scan_pos < m_source.length() && brace_count > 0) {
        if (m_source[block_scan_pos] == '{') brace_count++;
        else if (m_source[block_scan_pos] == '}') brace_count--;
        block_scan_pos++;
    }

    // If we found a matching brace, process the block
    if (brace_count == 0) {
        size_t content_start = opening_brace + 1;
        size_t content_end = block_scan_pos - 1;

        // Add the block's opening as a CHTL fragment (e.g., "script {")
        m_fragments.push_back({m_source.substr(next_block_keyword_pos, content_start - next_block_keyword_pos), FragmentType::CHTL});

        // Scan the content of the block
        m_cursor = content_start;
        if (block_type == "script") {
            scanScriptContent(content_end);
        } else if (block_type == "style") {
            scanStyleContent(content_end);
        }

        // Add the block's closing brace as a CHTL fragment
        m_fragments.push_back({"}", FragmentType::CHTL});
        m_cursor = block_scan_pos;
    } else {
        // Unmatched brace, treat the rest of the source as CHTL
        m_fragments.push_back({m_source.substr(next_block_keyword_pos), FragmentType::CHTL});
        m_cursor = m_source.length();
    }
}

void CHTLUnifiedScanner::scanStyleContent(size_t block_end) {
    if (m_cursor >= block_end) return;

    std::string block_content = m_source.substr(m_cursor, block_end - m_cursor);

    // Heuristics to detect CHTL features in CSS.
    bool contains_chtl_features =
        block_content.find('@') != std::string::npos ||      // @Style, @Var
        block_content.find('?') != std::string::npos ||      // Conditional
        block_content.find("delete") != std::string::npos || // delete keyword
        block_content.find("inherit") != std::string::npos;  // inherit keyword

    if (contains_chtl_features) {
        // If we detect CHTL features, we treat the whole block as a CHTL fragment.
        m_fragments.push_back({block_content, FragmentType::CHTL});
    } else {
        // Otherwise, we assume it's pure CSS.
        m_fragments.push_back({block_content, FragmentType::CSS});
    }
}

std::string CHTLUnifiedScanner::sanitizeCHTLJSBlock(const std::string& block_content) {
    std::string sanitized_content = block_content;
    size_t search_offset = 0;

    while (search_offset < sanitized_content.length()) {
        size_t colon_pos = sanitized_content.find(':', search_offset);
        if (colon_pos == std::string::npos) break;

        size_t value_start = colon_pos + 1;
        while (value_start < sanitized_content.length() && std::isspace(sanitized_content[value_start])) {
            value_start++;
        }
        if (value_start >= sanitized_content.length()) break;

        bool is_function = (sanitized_content.substr(value_start, 8) == "function") ||
                           (sanitized_content[value_start] == '(') ||
                           (sanitized_content.substr(value_start, 5) == "async");

        if (is_function) {
            size_t body_start_brace = sanitized_content.find('{', value_start);
            if (body_start_brace != std::string::npos) {
                int brace_count = 1;
                size_t body_end_brace = body_start_brace + 1;
                while (body_end_brace < sanitized_content.length() && brace_count > 0) {
                    if (sanitized_content[body_end_brace] == '{') brace_count++;
                    else if (sanitized_content[body_end_brace] == '}') brace_count--;
                    body_end_brace++;
                }

                if (brace_count == 0) {
                    std::string js_function = sanitized_content.substr(value_start, body_end_brace - value_start);
                    std::string placeholder_id = "_JS_PLACEHOLDER_" + std::to_string(m_placeholder_counter++) + "_";
                    m_fragments.push_back({js_function, FragmentType::JS, placeholder_id});
                    sanitized_content.replace(value_start, js_function.length(), placeholder_id);
                    search_offset = value_start + placeholder_id.length();
                    continue;
                }
            }
        }
        search_offset = colon_pos + 1;
    }
    return sanitized_content;
}

void CHTLUnifiedScanner::scanScriptContent(size_t block_end) {
    // This new implementation processes the script content as a stream,
    // creating JS and CHTL_JS fragments as it goes.

    while (m_cursor < block_end) {
        // Look for the next CHTL JS construct from the current position.
        size_t next_construct_pos = block_end;
        std::string found_keyword;

        // Check for {{
        size_t next_brace_pos = m_source.find("{{", m_cursor);
        if (next_brace_pos != std::string::npos && next_brace_pos < next_construct_pos) {
            next_construct_pos = next_brace_pos;
        }

        // Check for keywords
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t keyword_pos = m_source.find(keyword, m_cursor);
            if (keyword_pos != std::string::npos && keyword_pos < next_construct_pos) {
                next_construct_pos = keyword_pos;
                found_keyword = keyword;
            }
        }

        // If there's content before the next construct, it's JS.
        if (next_construct_pos > m_cursor) {
            m_fragments.push_back({m_source.substr(m_cursor, next_construct_pos - m_cursor), FragmentType::JS});
        }

        m_cursor = next_construct_pos;

        // If no more constructs were found, we're done with this script block.
        if (m_cursor == block_end) {
            break;
        }

        // Process the found construct
        if (m_source.substr(m_cursor, 2) == "{{") {
            size_t end_brace_pos = m_source.find("}}", m_cursor);
            if (end_brace_pos != std::string::npos && end_brace_pos < block_end) {
                size_t construct_end = end_brace_pos + 2;
                m_fragments.push_back({m_source.substr(m_cursor, construct_end - m_cursor), FragmentType::CHTL_JS});
                m_cursor = construct_end;
            } else { // Unmatched {{
                m_cursor += 2;
            }
        } else if (!found_keyword.empty()) {
            // It's a keyword block like Listen { ... }
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
                    // Extract the inner content of the block
                    std::string inner_content = m_source.substr(opening_brace + 1, end_scan - opening_brace - 2);
                    std::string sanitized_content = sanitizeCHTLJSBlock(inner_content);

                    // Reassemble the CHTL JS block with sanitized content
                    std::string full_block = m_source.substr(m_cursor, opening_brace + 1 - m_cursor) + sanitized_content + "}";
                    m_fragments.push_back({full_block, FragmentType::CHTL_JS});
                    m_cursor = end_scan;
                } else { // Unmatched {
                     m_cursor++;
                }
            } else { // Keyword without a brace
                m_cursor++;
            }
        }
    }
}

} // namespace CHTL
