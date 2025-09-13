#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>

namespace CHTL {

// This regex is designed to find top-level script and style blocks.
const std::regex CHTLUnifiedScanner::TAG_REGEX(R"((script|style)\s*\{)");
// This regex finds CHTL JS specific syntax.
// It looks for {{...}}, ->listen, ->delegate, animate {...}, or vir ...
const std::regex CHTLUnifiedScanner::CHTL_JS_REGEX(R"(\{\{[^}]*\}\}|->listen|->delegate|animate\s*\{|vir\s+\w+)");

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    findAndProcessTags();
    return chunks_;
}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholders() const {
    return placeholder_map_;
}

size_t CHTLUnifiedScanner::findMatchingBrace(size_t start_pos) {
    int brace_level = 1;
    size_t current_pos = start_pos;
    while (current_pos < source_.length() && brace_level > 0) {
        if (source_[current_pos] == '{') brace_level++;
        if (source_[current_pos] == '}') brace_level--;
        current_pos++;
    }
    return brace_level == 0 ? current_pos : std::string::npos;
}

void CHTLUnifiedScanner::findAndProcessTags() {
    auto it = std::sregex_iterator(source_.begin(), source_.end(), TAG_REGEX);
    auto end = std::sregex_iterator();
    size_t last_pos = 0;

    for (; it != end; ++it) {
        std::smatch match = *it;
        size_t tag_start_pos = match.position();

        size_t brace_open_pos = source_.find('{', tag_start_pos);
        if (brace_open_pos == std::string::npos) continue;

        size_t brace_close_pos = findMatchingBrace(brace_open_pos + 1);

        if (brace_close_pos != std::string::npos) {
            processChtlContent(last_pos, tag_start_pos);

            std::string block_content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);

            if (match.str(1) == "script") {
                processScriptBlock(block_content);
            } else if (match.str(1) == "style") {
                processStyleBlock(block_content);
            }

            last_pos = brace_close_pos;
        }
    }

    processChtlContent(last_pos, source_.length());
}

void CHTLUnifiedScanner::processChtlContent(size_t from, size_t to) {
    if (from < to) {
        std::string content = source_.substr(from, to - from);
        if (!content.empty() && content.find_first_not_of(" \t\n\r") != std::string::npos) {
            chunks_.push_back({ChunkType::CHTL, content});
        }
    }
}

void CHTLUnifiedScanner::processScriptBlock(const std::string& block_content) {
    if (block_content.empty()) {
        return;
    }

    auto it = std::sregex_iterator(block_content.begin(), block_content.end(), CHTL_JS_REGEX);
    auto end = std::sregex_iterator();

    std::string processed_content;
    size_t last_pos = 0;

    for (; it != end; ++it) {
        std::smatch match = *it;
        size_t match_pos = match.position();
        size_t match_len = match.length();

        // The content between the last match and this one is pure JS
        if (match_pos > last_pos) {
            std::string js_part = block_content.substr(last_pos, match_pos - last_pos);
            std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_part;
            processed_content += placeholder;
        }

        // Add the CHTL JS part itself
        processed_content += match.str();
        last_pos = match_pos + match_len;
    }

    // Handle any remaining pure JS at the end of the block
    if (last_pos < block_content.length()) {
        std::string js_part = block_content.substr(last_pos);
        std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
        placeholder_map_[placeholder] = js_part;
        processed_content += placeholder;
    }

    if (!processed_content.empty()) {
        chunks_.push_back({ChunkType::CHTL_JS, processed_content});
    }
}

void CHTLUnifiedScanner::processStyleBlock(const std::string& block_content) {
    // This is a global style block, so its content is pure CSS.
    if (!block_content.empty()) {
        chunks_.push_back({ChunkType::CSS, block_content});
    }
}

} // namespace CHTL
