#include "UnifiedScanner.h"
#include <sstream>

UnifiedScanner::UnifiedScanner(const std::string& source) : source(source) {}

// Helper to find the matching closing brace for a block
size_t find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') {
        return std::string::npos;
    }
    int depth = 1;
    for (size_t i = start_pos + 1; i < s.length(); ++i) {
        if (s[i] == '{') {
            depth++;
        } else if (s[i] == '}') {
            depth--;
            if (depth == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}


ScannedContent UnifiedScanner::scan() {
    ScannedContent result;
    std::stringstream chtl_ss;
    int brace_depth = 0;

    while (current < source.length()) {
        bool is_global_style = (brace_depth == 0 && source.substr(current, 5) == "style" && source.find('{', current) != std::string::npos);
        bool is_global_script = (brace_depth == 0 && source.substr(current, 6) == "script" && source.find('{', current) != std::string::npos);

        if (is_global_style || is_global_script) {
            size_t block_start = source.find('{', current);
            size_t block_end = find_matching_brace(source, block_start);

            if (block_end != std::string::npos) {
                std::string block_content = source.substr(block_start + 1, block_end - block_start - 1);
                if (is_global_style) {
                    result.css_blocks["global_style_" + std::to_string(css_block_count++)] = block_content;
                } else {
                    result.script_blocks["global_script_" + std::to_string(script_block_count++)] = block_content;
                }
                current = block_end + 1;
                continue; // Skip appending this block to chtl_ss
            }
        }

        // If not a global block, process normally
        if (source[current] == '{') brace_depth++;
        if (source[current] == '}') brace_depth--;
        chtl_ss << source[current];
        current++;
    }

    result.chtl_content = chtl_ss.str();
    return result;
}
