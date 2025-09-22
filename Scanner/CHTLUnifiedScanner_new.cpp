#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <regex>

namespace CHTL {

// CHTL JS keywords that signify the start of a CHTL JS block
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "Listen", "Delegate", "Animate", "Router", "Vir",
    "printMylove", "iNeverAway", "util", "ScriptLoader"
};

// CHTL features that can appear in a global style block
const std::vector<std::string> CHTL_STYLE_FEATURES = {
    "ThemeColor", "@Style", "delete", "inherit", "@Var", "@Element",
    "width", "height", "color", "background-color", "margin", "padding"
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

    // Look for the next special block (style or script)
    size_t script_pos = findNextKeyword("script", m_cursor);
    size_t style_pos = findNextKeyword("style", m_cursor);

    size_t next_block_pos = std::string::npos;
    bool is_script = false;
    
    if (script_pos != std::string::npos && style_pos != std::string::npos) {
        if (script_pos < style_pos) {
            next_block_pos = script_pos;
            is_script = true;
        } else {
            next_block_pos = style_pos;
        }
    } else if (script_pos != std::string::npos) {
        next_block_pos = script_pos;
        is_script = true;
    } else if (style_pos != std::string::npos) {
        next_block_pos = style_pos;
    }

    if (next_block_pos == std::string::npos) {
        // No more special blocks, the rest is CHTL
        if (last_pos < m_source.length()) {
            m_fragments.push_back({m_source.substr(last_pos), FragmentType::CHTL});
        }
        m_cursor = m_source.length();
        return;
    }

    // Add the CHTL part before the block
    if (next_block_pos > last_pos) {
        m_fragments.push_back({m_source.substr(last_pos, next_block_pos - last_pos), FragmentType::CHTL});
    }

    // Find the opening brace
    size_t opening_brace = m_source.find('{', next_block_pos);
    if (opening_brace == std::string::npos) {
        m_cursor = next_block_pos + (is_script ? 6 : 5); // Move past "script" or "style"
        return;
    }

    // Add the keyword and opening brace as CHTL
    m_fragments.push_back({m_source.substr(next_block_pos, opening_brace + 1 - next_block_pos), FragmentType::CHTL});

    // Find the matching closing brace for the block
    size_t block_end = findMatchingBrace(opening_brace);
    if (block_end == std::string::npos) {
        // Unmatched brace, treat the rest as CHTL
        m_fragments.push_back({m_source.substr(opening_brace + 1), FragmentType::CHTL});
        m_cursor = m_source.length();
        return;
    }

    m_cursor = opening_brace + 1;

    if (is_script) {
        scanScriptContent(block_end);
    } else {
        scanStyleContent(block_end);
    }

    m_fragments.push_back({m_source.substr(block_end, 1), FragmentType::CHTL}); // Add the closing brace
    m_cursor = block_end + 1;
}

void CHTLUnifiedScanner::scanStyleContent(size_t block_end) {
    std::string css_buffer;
    std::string chtl_buffer;

    auto finalize_css_buffer = [&]() {
        if (!css_buffer.empty()) {
            m_fragments.push_back({css_buffer, FragmentType::CSS});
            css_buffer.clear();
        }
    };

    auto finalize_chtl_buffer = [&]() {
        if (!chtl_buffer.empty()) {
            m_fragments.push_back({chtl_buffer, FragmentType::CHTL});
            chtl_buffer.clear();
        }
    };

    while (m_cursor < block_end) {
        // Look for CHTL-specific features in style blocks
        size_t next_feature_pos = findNextCHTLFeature(m_cursor, block_end, CHTL_STYLE_FEATURES);
        
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

        // Find the end of the CHTL feature
        size_t end_pos = findEndOfCHTLFeature(m_cursor, block_end);
        
        chtl_buffer += m_source.substr(feature_start, end_pos - feature_start);
        m_cursor = end_pos;
    }

    finalize_css_buffer();
    finalize_chtl_buffer();
}

void CHTLUnifiedScanner::scanScriptContent(size_t block_end) {
    // For script content, we need to carefully separate JS and CHTL JS
    // The key insight is that CHTL JS constructs are specific patterns:
    // 1. Keyword followed by { (like Listen { ... })
    // 2. {{ ... }} patterns for selectors
    
    std::string content = m_source.substr(m_cursor, block_end - m_cursor);
    
    // Split content into lines for easier processing
    std::vector<std::string> lines;
    std::string current_line;
    for (char c : content) {
        if (c == '\n') {
            lines.push_back(current_line);
            current_line.clear();
        } else {
            current_line += c;
        }
    }
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    
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

    auto finalize_chtl_js_buffer = [&]() {
        if (!chtl_js_buffer.empty()) {
            m_fragments.push_back({chtl_js_buffer, FragmentType::CHTL_JS});
            chtl_js_buffer.clear();
        }
    };
    
    bool in_chtl_js_construct = false;
    int brace_count = 0;
    
    for (const auto& line : lines) {
        std::string trimmed_line = line;
        // Remove leading whitespace
        trimmed_line.erase(0, trimmed_line.find_first_not_of(" \t"));
        
        // Check if this line starts a CHTL JS construct
        bool starts_chtl_js = false;
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            if (trimmed_line.find(keyword) == 0) {
                // Check if it's followed by whitespace and then {
                size_t after_keyword = keyword.length();
                while (after_keyword < trimmed_line.length() && std::isspace(trimmed_line[after_keyword])) {
                    after_keyword++;
                }
                if (after_keyword < trimmed_line.length() && trimmed_line[after_keyword] == '{') {
                    starts_chtl_js = true;
                    break;
                }
            }
        }
        
        // Check for {{ pattern
        if (trimmed_line.find("{{") != std::string::npos) {
            starts_chtl_js = true;
        }
        
        if (starts_chtl_js || in_chtl_js_construct) {
            if (!in_chtl_js_construct) {
                // Starting a CHTL JS construct
                finalize_js_buffer();
                in_chtl_js_construct = true;
                brace_count = 0;
            }
            
            chtl_js_buffer += line + "\n";
            
            // Count braces to know when construct ends
            for (char c : line) {
                if (c == '{') brace_count++;
                if (c == '}') brace_count--;
            }
            
            if (brace_count == 0 && in_chtl_js_construct) {
                // CHTL JS construct ended
                in_chtl_js_construct = false;
            }
        } else {
            // Regular JS code
            js_buffer += line + "\n";
        }
    }
    
    finalize_js_buffer();
    finalize_chtl_js_buffer();
    
    m_cursor = block_end;
}

// Helper functions
size_t CHTLUnifiedScanner::findNextKeyword(const std::string& keyword, size_t start_pos) {
    size_t pos = m_source.find(keyword, start_pos);
    while (pos != std::string::npos) {
        // Check if it's a whole word
        bool is_whole_word = true;
        if (pos > 0 && (std::isalnum(m_source[pos - 1]) || m_source[pos - 1] == '_')) {
            is_whole_word = false;
        }
        if (pos + keyword.length() < m_source.length() && 
            (std::isalnum(m_source[pos + keyword.length()]) || m_source[pos + keyword.length()] == '_')) {
            is_whole_word = false;
        }
        
        if (is_whole_word) {
            return pos;
        }
        
        pos = m_source.find(keyword, pos + 1);
    }
    return std::string::npos;
}

size_t CHTLUnifiedScanner::findMatchingBrace(size_t open_pos) {
    int brace_count = 1;
    size_t pos = open_pos + 1;
    
    while (pos < m_source.length() && brace_count > 0) {
        if (m_source[pos] == '{') {
            brace_count++;
        } else if (m_source[pos] == '}') {
            brace_count--;
        }
        pos++;
    }
    
    return (brace_count == 0) ? pos - 1 : std::string::npos;
}

size_t CHTLUnifiedScanner::findNextCHTLFeature(size_t start_pos, size_t end_pos, const std::vector<std::string>& features) {
    size_t next_pos = std::string::npos;
    
    for (const auto& feature : features) {
        size_t pos = m_source.find(feature, start_pos);
        if (pos != std::string::npos && pos < end_pos) {
            if (next_pos == std::string::npos) {
                next_pos = pos;
            } else {
                next_pos = std::min(next_pos, pos);
            }
        }
    }
    
    // Also look for operators that might indicate CHTL features
    size_t operator_pos = m_source.find_first_of("+-*/?@$", start_pos);
    if (operator_pos != std::string::npos && operator_pos < end_pos) {
        // Check if it's not part of a CSS calc() function
        if (m_source[operator_pos] == '@' || 
            (m_source[operator_pos] != 'c' || m_source.find("calc(", operator_pos - 4, 5) == std::string::npos)) {
            if (next_pos == std::string::npos) {
                next_pos = operator_pos;
            } else {
                next_pos = std::min(next_pos, operator_pos);
            }
        }
    }
    
    return next_pos;
}

size_t CHTLUnifiedScanner::findNextCHTLJSConstruct(size_t start_pos, size_t end_pos) {
    // This function is not used in the new implementation
    return std::string::npos;
}

size_t CHTLUnifiedScanner::findEndOfCHTLFeature(size_t start_pos, size_t end_pos) {
    // Look for semicolon, comma, or closing brace
    size_t semicolon_pos = m_source.find(';', start_pos);
    size_t comma_pos = m_source.find(',', start_pos);
    size_t brace_pos = m_source.find('}', start_pos);
    
    size_t end_pos_found = std::string::npos;
    if (semicolon_pos != std::string::npos && semicolon_pos < end_pos) {
        end_pos_found = semicolon_pos;
    }
    if (comma_pos != std::string::npos && comma_pos < end_pos) {
        if (end_pos_found == std::string::npos) {
            end_pos_found = comma_pos;
        } else {
            end_pos_found = std::min(end_pos_found, comma_pos);
        }
    }
    if (brace_pos != std::string::npos && brace_pos < end_pos) {
        if (end_pos_found == std::string::npos) {
            end_pos_found = brace_pos;
        } else {
            end_pos_found = std::min(end_pos_found, brace_pos);
        }
    }
    
    if (end_pos_found == std::string::npos) {
        return end_pos;
    }
    
    return end_pos_found + 1; // Include the terminator
}

size_t CHTLUnifiedScanner::findEndOfCHTLJSConstruct(size_t start_pos, size_t end_pos) {
    // This function is not used in the new implementation
    return std::string::npos;
}

} // namespace CHTL