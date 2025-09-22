#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source, std::shared_ptr<Configuration> config)
    : m_source(source), m_config(config) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    m_fragments.clear();
    m_cursor = 0;
    m_placeholder_counter = 0;

    // Handle 'use' statements at the beginning of the file
    while (m_cursor < m_source.length()) {
        // Skip leading whitespace
        while (m_cursor < m_source.length() && std::isspace(m_source[m_cursor])) {
            m_cursor++;
        }

        if (m_source.substr(m_cursor, 3) == "use") {
            size_t line_end = m_source.find(';', m_cursor);
            if (line_end != std::string::npos) {
                std::string directive = m_source.substr(m_cursor, line_end - m_cursor + 1);
                m_fragments.push_back({directive, FragmentType::DIRECTIVE});
                m_cursor = line_end + 1;
            } else {
                // No semicolon found, stop processing directives
                break;
            }
        } else {
            // Not a 'use' statement, break to main scanning
            break;
        }
    }

    while (m_cursor < m_source.length()) {
        scanSource();
    }

    return m_fragments;
}

void CHTLUnifiedScanner::scanSource() {
    size_t last_pos = m_cursor;

    // Define the block types we're looking for
    struct BlockType {
        std::string keyword;
        size_t position;
        // Handler now accepts the block header content
        std::function<void(CHTLUnifiedScanner*, size_t, const std::string&)> handler;
    };

    std::vector<BlockType> block_types;
    // Populate block_types from configuration
    for (const auto& keyword : m_config->keyword_map["KEYWORD_SCRIPT"]) {
        block_types.push_back(BlockType{keyword, std::string::npos, &CHTLUnifiedScanner::scanScriptContent});
    }
    for (const auto& keyword : m_config->keyword_map["KEYWORD_STYLE"]) {
        block_types.push_back(BlockType{keyword, std::string::npos, &CHTLUnifiedScanner::scanStyleContent});
    }
    for (const auto& keyword : m_config->keyword_map["KEYWORD_ORIGIN"]) {
        block_types.push_back(BlockType{keyword, std::string::npos, &CHTLUnifiedScanner::scanOriginContent});
    }


    // Find the earliest occurrence of any block type
    BlockType* next_block = nullptr;
    for (auto& type : block_types) {
        type.position = m_source.find(type.keyword, m_cursor);
        if (type.position != std::string::npos) {
            if (next_block == nullptr || type.position < next_block->position) {
                next_block = &type;
            }
        }
    }

    if (next_block == nullptr) {
        // No more blocks, the rest is CHTL
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length(); // End the loop
        return;
    }

    // Found a keyword. Find its block.
    size_t opening_brace = m_source.find('{', next_block->position);
    if (opening_brace == std::string::npos) {
        // Malformed block, skip the keyword and continue
        m_cursor = next_block->position + next_block->keyword.length();
        return;
    }

    // Add the CHTL part before the block keyword
    if (next_block->position > last_pos) {
        m_fragments.push_back({m_source.substr(last_pos, next_block->position - last_pos), FragmentType::CHTL});
    }

    // The part from the keyword to the brace is now considered part of the block's definition,
    // not raw CHTL. We will capture this "header" and pass it to the handler.
    // We push the keyword and header as a single CHTL fragment for now.
    m_fragments.push_back({m_source.substr(next_block->position, opening_brace - next_block->position), FragmentType::CHTL});
    m_fragments.push_back({m_source.substr(opening_brace, 1), FragmentType::CHTL});


    // Find the matching closing brace for the block.
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

        // The "header" is the part between the keyword and the opening brace
        size_t header_start = next_block->position + next_block->keyword.length();
        std::string header = m_source.substr(header_start, opening_brace - header_start);

        m_cursor = opening_brace + 1;

        // Call the handler for the specific block type, passing the header
        next_block->handler(this, block_end, header);

        m_fragments.push_back({m_source.substr(block_end, 1), FragmentType::CHTL}); // Add the closing brace
        m_cursor = block_end + 1;
    } else {
        // Unmatched brace, treat from last_pos to end as CHTL
        m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        m_cursor = m_source.length();
    }
}

void CHTLUnifiedScanner::scanStyleContent(size_t block_end, const std::string& header) {
    if (m_cursor < block_end) {
        // Style content is treated as CHTL for the CHTL compiler to process.
        std::string content = m_source.substr(m_cursor, block_end - m_cursor);
        m_fragments.push_back({content, FragmentType::CHTL});
        m_cursor = block_end;
    }
}

void CHTLUnifiedScanner::scanOriginContent(size_t block_end, const std::string& header) {
    FragmentType type = FragmentType::UNKNOWN;
    std::string trimmed_header = header;
    // Basic trim
    size_t first = trimmed_header.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        trimmed_header = "";
    }
    size_t last = trimmed_header.find_last_not_of(" \t\n\r");
    trimmed_header = trimmed_header.substr(first, (last - first + 1));

    if (trimmed_header.rfind("@Html", 0) == 0) { // check starts with
        type = FragmentType::HTML;
    } else if (trimmed_header.rfind("@Style", 0) == 0) {
        type = FragmentType::CSS;
    } else if (trimmed_header.rfind("@JavaScript", 0) == 0) {
        type = FragmentType::JS;
    }

    if (m_cursor < block_end) {
        std::string content = m_source.substr(m_cursor, block_end - m_cursor);
        m_fragments.push_back({content, type});
        m_cursor = block_end;
    }
}

void CHTLUnifiedScanner::scanScriptContent(size_t block_end, const std::string& header) {
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
        if (m_config && m_config->keyword_map.count("CHTLJS_FUNCTIONS")) {
            for (const auto& keyword : m_config->keyword_map.at("CHTLJS_FUNCTIONS")) {
                if (m_source.rfind(keyword, m_cursor) == m_cursor) { // check starts with
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
        }

        if (!keyword_found) {
            // If it wasn't a recognized CHTL JS keyword construct, it might be something else.
            // For now, we just consume the initial part and continue. This part of the
            // scanner is still basic and needs improvement based on the spec's placeholder mechanism.
            if (m_source.substr(construct_start, 2) == "{{") m_cursor = construct_start + 2;
            else if (m_source.substr(construct_start, 3) == "&->") m_cursor = construct_start + 3;
            else if (m_source.substr(construct_start, 2) == "->") m_cursor = construct_start + 2;
        }

        chtl_js_buffer += m_source.substr(construct_start, m_cursor - construct_start);
    }

    finalize_js_buffer(); // Add any remaining JS
    if (!chtl_js_buffer.empty()) {
        m_fragments.push_back({chtl_js_buffer, FragmentType::CHTL_JS});
    }
}

} // namespace CHTL
