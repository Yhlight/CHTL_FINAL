#include "Scanner/CHTLUnifiedScanner.h"
#include <vector>
#include <string>
#include <regex>
#include <sstream>

namespace CHTL {

// Helper function to find the matching closing brace, handling nested braces.
size_t find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    for (size_t i = start_pos + 1; i < s.length(); ++i) {
        if (s[i] == '{') {
            brace_level++;
        } else if (s[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source), current_(0), placeholder_id_(0) {}

std::string CHTLUnifiedScanner::process_script_block(const std::string& content) {
    // This regex finds CHTL JS specific syntax.
    std::regex chtl_js_finder(R"(\{\{[^}]*\}\}|->|\b(listen|animate|delegate|vir|router)\b)");

    std::stringstream result_stream;
    auto it = std::sregex_iterator(content.begin(), content.end(), chtl_js_finder);
    auto end = std::sregex_iterator();

    size_t last_pos = 0;
    for (; it != end; ++it) {
        std::smatch match = *it;
        if (match.position() > last_pos) {
            std::string js_part = content.substr(last_pos, match.position() - last_pos);
            if (!js_part.empty()) {
                std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
                placeholder_map_[placeholder] = js_part;
                result_stream << placeholder;
            }
        }
        result_stream << match.str();
        last_pos = match.position() + match.length();
    }

    if (last_pos < content.length()) {
        std::string js_part = content.substr(last_pos);
        if (!js_part.empty()) {
            std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
            placeholder_map_[placeholder] = js_part;
            result_stream << placeholder;
        }
    }

    return result_stream.str();
}

std::string CHTLUnifiedScanner::process_style_block(const std::string& content) {
    // For now, the scanner's job is just to separate the block.
    // The CHTL compiler will handle parsing CHTL features within it.
    // Therefore, we just return the content as is.
    return content;
}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    std::regex block_finder(R"(\b(style|script)\b\s*\{)");
    std::smatch match;

    std::string remaining_source = source_;
    size_t offset = 0;

    while (std::regex_search(remaining_source, match, block_finder)) {
        if (match.position() > 0) {
            chunks_.push_back({ChunkType::CHTL, remaining_source.substr(0, match.position())});
        }

        std::string block_type = match[1].str();
        size_t block_start_pos = offset + match.prefix().length() + match.length(1);
        while(isspace(source_[block_start_pos])) block_start_pos++;

        size_t block_end_pos = find_matching_brace(source_, block_start_pos);

        if (block_end_pos != std::string::npos) {
            std::string block_content = source_.substr(block_start_pos + 1, block_end_pos - block_start_pos - 1);

            if (block_type == "script") {
                std::string processed_content = process_script_block(block_content);
                chunks_.push_back({ChunkType::ChtlJs, processed_content});
            } else if (block_type == "style") {
                std::string processed_content = process_style_block(block_content);
                chunks_.push_back({ChunkType::Css, processed_content});
            }

            offset = block_end_pos + 1;
            if (offset >= source_.length()) {
                remaining_source = "";
            } else {
                remaining_source = source_.substr(offset);
            }
        } else {
            chunks_.push_back({ChunkType::CHTL, remaining_source});
            remaining_source = "";
        }
    }

    if (!remaining_source.empty()) {
        chunks_.push_back({ChunkType::CHTL, remaining_source});
    }

    return chunks_;
}

} // namespace CHTL
