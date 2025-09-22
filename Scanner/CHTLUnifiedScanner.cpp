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

    // Find the next "global" script or style block
    size_t script_pos = m_source.find("script", m_cursor);
    size_t style_pos = m_source.find("style", m_cursor);

    // Heuristic: A global block is a keyword preceded by whitespace (or start of file)
    // and followed by an opening brace (potentially after more whitespace).
    auto find_global_block = [&](const std::string& keyword, size_t start_pos) {
        size_t pos = m_source.find(keyword, start_pos);
        while (pos != std::string::npos) {
            // Check for preceding character
            if (pos != 0 && !std::isspace(m_source[pos - 1])) {
                pos = m_source.find(keyword, pos + 1);
                continue;
            }
            // Check for following brace
            size_t brace_pos = m_source.find('{', pos + keyword.length());
            if (brace_pos == std::string::npos) {
                return std::string::npos; // No brace found after this keyword
            }
            // Check that everything between keyword and brace is whitespace
            bool all_whitespace = true;
            for (size_t i = pos + keyword.length(); i < brace_pos; ++i) {
                if (!std::isspace(m_source[i])) {
                    all_whitespace = false;
                    break;
                }
            }
            if (all_whitespace) {
                return pos; // This is a valid global block
            }
            // Not a global block, continue searching
            pos = m_source.find(keyword, pos + 1);
        }
        return std::string::npos;
    };

    script_pos = find_global_block("script", m_cursor);
    style_pos = find_global_block("style", m_cursor);

    size_t next_block_pos = std::min(script_pos, style_pos);

    if (next_block_pos == std::string::npos) {
        // No more global blocks, the rest is CHTL for the CHTL parser
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length();
        return;
    }

    // Add the CHTL part before the global block
    if (next_block_pos > last_pos) {
        m_fragments.push_back({m_source.substr(last_pos, next_block_pos - last_pos), FragmentType::CHTL});
    }

    size_t opening_brace = m_source.find('{', next_block_pos);
    if (opening_brace == std::string::npos) {
        m_cursor = next_block_pos + 1; // Skip broken keyword
        return;
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

        // The block keyword and opening brace are considered delimiters and are discarded by the scanner.
        // The content inside is passed to the specialized scanner.
        m_cursor = opening_brace + 1;

        if (next_block_pos == script_pos) {
            scanScriptContent(block_end);
        } else {
            scanStyleContent(block_end);
        }

        // Move cursor past the entire block
        m_cursor = block_end + 1;

    } else {
        // Unmatched brace, treat the rest as CHTL
        m_fragments.push_back({m_source.substr(next_block_pos), FragmentType::CHTL});
        m_cursor = m_source.length();
    }
}

void CHTLUnifiedScanner::scanStyleContent(size_t block_end) {
    std::string css_buffer;
    size_t last_flush_pos = m_cursor;

    while (m_cursor < block_end) {
        // Find the next declaration or block boundary, making sure it's within the block
        size_t next_semicolon = m_source.find(';', m_cursor);
        if (next_semicolon >= block_end) next_semicolon = std::string::npos;

        size_t next_brace = m_source.find_first_of("{}", m_cursor);
        if (next_brace >= block_end) next_brace = std::string::npos;

        // If no more delimiters, we're done with this loop
        if (next_semicolon == std::string::npos && next_brace == std::string::npos) {
            break;
        }

        // If the next delimiter is a brace, or if there are no semicolons, handle the brace
        if (next_brace != std::string::npos && (next_brace < next_semicolon || next_semicolon == std::string::npos)) {
            // Flush everything up to and including the brace, then continue
            css_buffer += m_source.substr(last_flush_pos, next_brace - last_flush_pos + 1);
            m_cursor = next_brace + 1;
            last_flush_pos = m_cursor;
            continue;
        }

        // Otherwise, we have a declaration ending with a semicolon
        size_t decl_end = next_semicolon;
        std::string declaration = m_source.substr(m_cursor, decl_end - m_cursor);

        // More robust CHTL feature check
        bool is_chtl = (declaration.find('+') != std::string::npos && declaration.find("calc") == std::string::npos) ||
                       (declaration.find('?') != std::string::npos) ||
                       (declaration.find("@Style") != std::string::npos) ||
                       (declaration.find("@Var") != std::string::npos) ||
                       (declaration.find("delete") != std::string::npos);

        if (is_chtl) {
            // Flush the pure CSS before this declaration
            css_buffer += m_source.substr(last_flush_pos, m_cursor - last_flush_pos);

            // Create a CHTL fragment for the full declaration
            std::string chtl_content = declaration + ";";
            std::string placeholder = "_CHTL_CSS_PLACEHOLDER_" + std::to_string(m_placeholder_counter++) + "_";
            m_fragments.push_back({chtl_content, FragmentType::CHTL, placeholder});

            // Add placeholder to the css buffer. The semicolon is part of the placeholder now for better formatting.
            css_buffer += placeholder + ";";

            // Move cursor past this declaration
            m_cursor = decl_end + 1;
            last_flush_pos = m_cursor;
        } else {
            // It's a pure CSS declaration, so just advance the cursor.
            // The content will be flushed later.
            m_cursor = decl_end + 1;
        }
    }

    // After the loop, flush any remaining pure CSS content
    if (last_flush_pos < block_end) {
        css_buffer += m_source.substr(last_flush_pos, block_end - last_flush_pos);
    }

    if (!css_buffer.empty()) {
        m_fragments.push_back({css_buffer, FragmentType::CSS});
    }

    // Ensure the main cursor is at the end of the block for the parent scanner
    m_cursor = block_end;
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
