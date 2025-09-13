#include "Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <cctype>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source), current_(0), placeholder_id_(0) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = current_;

    while (current_ < source_.length()) {
        if (source_.substr(current_, 7) == "<script") {
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleScriptTag();
            last_pos = current_;
        } else if (source_.substr(current_, 6) == "<style") {
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleStyleTag();
            last_pos = current_;
        } else if (isalnum(source_[current_])) {
            // Potential start of a CHTL block like 'style {' or 'script {'
            // The actual handling will be inside handleChtlBlock.
            // This is a simplified check to find a potential block.
            size_t word_end = current_;
            while (word_end < source_.length() && (isalnum(source_[word_end]) || source_[word_end] == '_')) {
                word_end++;
            }
            std::string word = source_.substr(current_, word_end - current_);
            if (word == "style" || word == "script") {
                 size_t brace_pos = source_.find('{', word_end);
                 if (brace_pos != std::string::npos) {
                     bool only_whitespace = true;
                     for(size_t i = word_end; i < brace_pos; ++i) {
                         if (!isspace(source_[i])) {
                             only_whitespace = false;
                             break;
                         }
                     }
                     if (only_whitespace) {
                        if (current_ > last_pos) {
                            chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
                        }
                        handleChtlBlock();
                        last_pos = current_;
                        continue; // CHTL block handled, continue loop
                     }
                 }
            }
            current_ = word_end; // Move past the word
        }
        else {
            current_++;
        }
    }

    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
    }
}

void CHTLUnifiedScanner::handleScriptTag() {
    size_t tag_end_pos = source_.find('>', current_);
    if (tag_end_pos == std::string::npos) {
        // Malformed tag, treat the rest of the file as CHTL for now.
        current_ = source_.length();
        return;
    }

    size_t script_end_pos = source_.find("</script>", tag_end_pos);
    if (script_end_pos == std::string::npos) {
        // Malformed, no closing tag found.
        current_ = source_.length();
        return;
    }

    std::string content = source_.substr(tag_end_pos + 1, script_end_pos - (tag_end_pos + 1));
    chunks_.push_back({ChunkType::JavaScript, content});

    // Advance current_ past the </script> tag
    current_ = script_end_pos + 9;
}

void CHTLUnifiedScanner::handleStyleTag() {
    size_t tag_end_pos = source_.find('>', current_);
    if (tag_end_pos == std::string::npos) {
        // Malformed tag.
        current_ = source_.length();
        return;
    }

    size_t style_end_pos = source_.find("</style>", tag_end_pos);
    if (style_end_pos == std::string::npos) {
        // Malformed, no closing tag found.
        current_ = source_.length();
        return;
    }

    std::string content = source_.substr(tag_end_pos + 1, style_end_pos - (tag_end_pos + 1));
    chunks_.push_back({ChunkType::Css, content});

    // Advance current_ past the </style> tag
    current_ = style_end_pos + 8;
}

void CHTLUnifiedScanner::handleChtlBlock() {
    // We assume process() has put `current_` at the start of the keyword ('style' or 'script')
    size_t keyword_start = current_;
    size_t brace_open_pos = source_.find('{', keyword_start);
    if (brace_open_pos == std::string::npos) {
        current_ = source_.length(); // Malformed
        return;
    }

    std::string keyword;
    size_t word_end = keyword_start;
    while (word_end < brace_open_pos && isalnum(source_[word_end])) {
        word_end++;
    }
    keyword = source_.substr(keyword_start, word_end - keyword_start);

    // Find matching closing brace
    int brace_level = 1;
    size_t content_end_pos = brace_open_pos + 1;
    while(content_end_pos < source_.length() && brace_level > 0) {
        if(source_[content_end_pos] == '{') brace_level++;
        if(source_[content_end_pos] == '}') brace_level--;
        content_end_pos++;
    }

    if (brace_level != 0) {
        current_ = source_.length(); // Malformed
        return;
    }

    if (keyword == "style") {
        std::string full_block = source_.substr(keyword_start, content_end_pos - keyword_start);
        chunks_.push_back({ChunkType::CHTL, full_block});
    } else if (keyword == "script") {
        std::string content = source_.substr(brace_open_pos + 1, content_end_pos - brace_open_pos - 2);
        scanScriptContent(content);
    }

    current_ = content_end_pos;
}

void CHTLUnifiedScanner::scanScriptContent(const std::string& content) {
    const std::vector<std::string> cjs_keywords = {"listen", "delegate", "animate", "vir"};
    size_t local_pos = 0;
    size_t last_pos = 0;

    while (local_pos < content.length()) {
        size_t next_cjs_pos = std::string::npos;
        std::string found_keyword = "";
        bool is_brace_construct = false;

        // Find the earliest CHTL JS construct
        size_t brace_pos = content.find("{{", local_pos);
        if (brace_pos != std::string::npos) {
            next_cjs_pos = brace_pos;
            is_brace_construct = true;
        }

        for (const auto& keyword : cjs_keywords) {
            size_t keyword_pos = content.find(keyword, local_pos);
            if (keyword_pos != std::string::npos) {
                if (keyword_pos < next_cjs_pos) {
                    next_cjs_pos = keyword_pos;
                    found_keyword = keyword;
                    is_brace_construct = false;
                }
            }
        }

        if (next_cjs_pos == std::string::npos) {
            break; // No more CHTL JS constructs
        }

        if (next_cjs_pos > last_pos) {
            std::string js_content = content.substr(last_pos, next_cjs_pos - last_pos);
            std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_content;
            chunks_.push_back({ChunkType::Placeholder, placeholder});
        }

        size_t cjs_start_pos = next_cjs_pos;
        size_t cjs_end_pos = cjs_start_pos;

        if (is_brace_construct) {
            cjs_end_pos = content.find("}}", cjs_start_pos);
            if (cjs_end_pos != std::string::npos) cjs_end_pos += 2;
        } else {
            size_t brace_open = content.find('{', cjs_start_pos + found_keyword.length());
            if (brace_open != std::string::npos) {
                int brace_level = 1;
                cjs_end_pos = brace_open + 1;
                while (cjs_end_pos < content.length() && brace_level > 0) {
                    if (content[cjs_end_pos] == '{') brace_level++;
                    if (content[cjs_end_pos] == '}') brace_level--;
                    cjs_end_pos++;
                }
            }
        }

        if (cjs_end_pos <= cjs_start_pos) {
            local_pos = next_cjs_pos + 1;
            continue;
        }

        chunks_.push_back({ChunkType::ChtlJs, content.substr(cjs_start_pos, cjs_end_pos - cjs_start_pos)});
        local_pos = cjs_end_pos;
        last_pos = local_pos;
    }

    if (last_pos < content.length()) {
        std::string js_content = content.substr(last_pos);
        if (!js_content.empty()) {
            std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_content;
            chunks_.push_back({ChunkType::Placeholder, placeholder});
        }
    }
}

} // namespace CHTL
