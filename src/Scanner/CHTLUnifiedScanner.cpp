#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>

namespace CHTL {

namespace {
// Helper to find a standalone keyword, checking for word boundaries.
size_t find_keyword(const std::string& source, const std::string& keyword, size_t offset) {
    size_t pos = source.find(keyword, offset);
    while (pos != std::string::npos) {
        bool start_ok = (pos == 0) || !isalnum(source[pos - 1]);
        bool end_ok = (pos + keyword.length() == source.length()) || !isalnum(source[pos + keyword.length()]);

        if (start_ok && end_ok) {
            return pos;
        }
        pos = source.find(keyword, pos + 1);
    }
    return std::string::npos;
}

// Helper to find the matching closing brace, correctly handling nested braces.
size_t find_matching_brace(const std::string& str, size_t start_pos) {
    if (start_pos >= str.length() || str[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    size_t pos = start_pos + 1;
    while (pos < str.length()) {
        if (str[pos] == '{') {
            brace_level++;
        } else if (str[pos] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return pos + 1; // Return position right after the closing brace
            }
        }
        pos++;
    }
    return std::string::npos; // Unmatched brace
}
} // anonymous namespace

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    current_ = 0;

    while (current_ < source_.length()) {
        size_t script_pos = find_keyword(source_, "script", current_);
        size_t style_pos = find_keyword(source_, "style", current_);

        size_t next_block_pos = std::string::npos;
        bool is_script = false;

        if (script_pos != std::string::npos && (style_pos == std::string::npos || script_pos < style_pos)) {
            next_block_pos = script_pos;
            is_script = true;
        } else if (style_pos != std::string::npos) {
            next_block_pos = style_pos;
            is_script = false;
        } else {
            // No more special blocks found
            break;
        }

        size_t keyword_end = next_block_pos + (is_script ? 6 : 5);
        size_t brace_open_pos = source_.find('{', keyword_end);

        if (brace_open_pos != std::string::npos) {
            bool only_whitespace = true;
            for (size_t i = keyword_end; i < brace_open_pos; ++i) {
                if (!isspace(source_[i])) {
                    only_whitespace = false;
                    break;
                }
            }

            if (only_whitespace) {
                // Found a block. Add the preceding text as a CHTL chunk.
                if (next_block_pos > last_pos) {
                    chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, next_block_pos - last_pos)});
                }

                // Process the block itself
                current_ = brace_open_pos;
                if (is_script) {
                    handleScriptTag();
                } else {
                    handleStyleTag();
                }
                last_pos = current_;
                continue;
            }
        }
        // If we are here, it means we found a keyword but not a valid block after it.
        // Advance past the found keyword to avoid an infinite loop.
        current_ = keyword_end;
    }

    // Add any remaining content as a final CHTL chunk.
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}

void CHTLUnifiedScanner::handleScriptTag() {
    size_t block_start_pos = current_;
    size_t block_end_pos = find_matching_brace(source_, block_start_pos);

    if (block_end_pos == std::string::npos) {
        // Error: Unmatched opening brace for script block.
        current_ = source_.length();
        return;
    }

    std::string content = source_.substr(block_start_pos + 1, block_end_pos - block_start_pos - 2);
    size_t content_pos = 0;
    size_t last_content_pos = 0;

    while (content_pos < content.length()) {
        size_t chtl_js_start = content.find("{{", content_pos);

        if (chtl_js_start == std::string::npos) {
            // No more CHTL JS blocks in the content.
            break;
        }

        if (chtl_js_start > last_content_pos) {
            chunks_.push_back({ChunkType::JavaScript, content.substr(last_content_pos, chtl_js_start - last_content_pos)});
        }

        int brace_level = 1;
        size_t chtl_js_end_search_pos = chtl_js_start + 2;
        size_t chtl_js_end = std::string::npos;

        while (chtl_js_end_search_pos + 1 < content.length()) {
            if (content.substr(chtl_js_end_search_pos, 2) == "{{") {
                brace_level++;
                chtl_js_end_search_pos += 2;
            } else if (content.substr(chtl_js_end_search_pos, 2) == "}}") {
                brace_level--;
                if (brace_level == 0) {
                    chtl_js_end = chtl_js_end_search_pos + 2;
                    break;
                }
                chtl_js_end_search_pos += 2;
            } else {
                chtl_js_end_search_pos++;
            }
        }

        if (chtl_js_end != std::string::npos) {
            chunks_.push_back({ChunkType::ChtlJs, content.substr(chtl_js_start, chtl_js_end - chtl_js_start)});
            content_pos = chtl_js_end;
            last_content_pos = content_pos;
        } else {
            // Unmatched '{{', treat the rest as JS and stop searching for CHTL JS.
            break;
        }
    }

    if (last_content_pos < content.length()) {
        chunks_.push_back({ChunkType::JavaScript, content.substr(last_content_pos)});
    }

    current_ = block_end_pos;
}

void CHTLUnifiedScanner::handleStyleTag() {
    size_t brace_end_pos = find_matching_brace(source_, current_);
    if (brace_end_pos != std::string::npos) {
        std::string content = source_.substr(current_ + 1, brace_end_pos - current_ - 2);
        chunks_.push_back({ChunkType::Css, content});
        current_ = brace_end_pos;
    } else {
        // Error: Unmatched opening brace for style block.
        current_ = source_.length();
    }
}

} // namespace CHTL
