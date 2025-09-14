#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    while (current_ < source_.length()) {
        size_t script_pos = source_.find("script", current_);
        size_t style_pos = source_.find("style", current_);

        size_t next_keyword_pos = std::min(script_pos, style_pos);

        if (next_keyword_pos == std::string::npos) {
            if (current_ < source_.length()) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(current_)});
            }
            break;
        }

        size_t brace_pos = source_.find('{', next_keyword_pos);
        if (brace_pos == std::string::npos) {
             if (current_ < source_.length()) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(current_)});
            }
            break;
        }

        bool only_whitespace = true;
        for (size_t i = next_keyword_pos + (next_keyword_pos == script_pos ? 6 : 5); i < brace_pos; ++i) {
            if (!isspace(source_[i])) {
                only_whitespace = false;
                break;
            }
        }

        if (!only_whitespace) {
            // This is not a script/style block, treat as CHTL and continue search
            size_t chunk_end = next_keyword_pos + 1;
            chunks_.push_back({ChunkType::CHTL, source_.substr(current_, chunk_end - current_)});
            current_ = chunk_end;
            continue;
        }

        // Add the CHTL content before the special block
        if (next_keyword_pos > current_) {
            chunks_.push_back({ChunkType::CHTL, source_.substr(current_, next_keyword_pos - current_)});
        }

        current_ = next_keyword_pos;

        if (current_ == script_pos) {
            handleScriptTag();
        } else if (current_ == style_pos) {
            handleStyleTag();
        }
    }
}

void CHTLUnifiedScanner::handleScriptTag() {
    size_t keyword_end = current_ + 6; // "script"
    size_t block_start_pos = source_.find('{', keyword_end);

    int brace_level = 1;
    size_t block_end_pos = block_start_pos + 1;
    while(block_end_pos < source_.length() && brace_level > 0) {
        if(source_[block_end_pos] == '{') brace_level++;
        if(source_[block_end_pos] == '}') brace_level--;
        block_end_pos++;
    }

    if (brace_level != 0) {
        current_ = block_start_pos;
        return;
    }

    size_t script_content_start = block_start_pos + 1;
    size_t script_content_end = block_end_pos - 1;
    size_t inner_pos = script_content_start;

    while(inner_pos < script_content_end) {
        size_t next_cjs_pos = std::string::npos;
        std::string cjs_keyword;
        std::vector<std::string> keywords = {"{{", "listen", "animate", "delegate"};
        for (const auto& kw : keywords) {
            size_t pos = source_.find(kw, inner_pos);
            if (pos != std::string::npos && pos < script_content_end && pos < next_cjs_pos) {
                next_cjs_pos = pos;
                cjs_keyword = kw;
            }
        }

        if (next_cjs_pos == std::string::npos) break;

        if (next_cjs_pos > inner_pos) {
            std::string js_content = source_.substr(inner_pos, next_cjs_pos - inner_pos);
            if(!js_content.empty()){
                std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
                placeholder_map_[placeholder] = js_content;
                chunks_.push_back({ChunkType::Placeholder, placeholder});
            }
        }

        size_t cjs_end_pos = std::string::npos;
        if (cjs_keyword == "{{") {
            cjs_end_pos = source_.find("}}", next_cjs_pos);
            if (cjs_end_pos != std::string::npos) cjs_end_pos += 2;
        } else {
            size_t cjs_brace_start = source_.find("{", next_cjs_pos);
            if (cjs_brace_start != std::string::npos) {
                int cjs_brace_level = 1;
                cjs_end_pos = cjs_brace_start + 1;
                while(cjs_end_pos < script_content_end && cjs_brace_level > 0) {
                    if(source_[cjs_end_pos] == '{') cjs_brace_level++;
                    if(source_[cjs_end_pos] == '}') cjs_brace_level--;
                    cjs_end_pos++;
                }
                if (cjs_brace_level != 0) cjs_end_pos = std::string::npos;
            }
        }

        if (cjs_end_pos != std::string::npos) {
            std::string cjs_content = source_.substr(next_cjs_pos, cjs_end_pos - next_cjs_pos);
            chunks_.push_back({ChunkType::ChtlJs, cjs_content});
            inner_pos = cjs_end_pos;
        } else {
            break;
        }
    }

    if (inner_pos < script_content_end) {
        std::string js_content = source_.substr(inner_pos, script_content_end - inner_pos);
        if (!js_content.empty()) {
            std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_content;
            chunks_.push_back({ChunkType::Placeholder, placeholder});
        }
    }

    current_ = block_end_pos;
}

void CHTLUnifiedScanner::handleStyleTag() {
    size_t keyword_end = current_ + 5; // "style"
    size_t block_start_pos = source_.find('{', keyword_end);

    int brace_level = 1;
    size_t content_start_pos = block_start_pos + 1;
    size_t end_pos = content_start_pos;
    while(end_pos < source_.length() && brace_level > 0) {
        if(source_[end_pos] == '{') brace_level++;
        if(source_[end_pos] == '}') brace_level--;
        end_pos++;
    }

    if (brace_level == 0) {
        std::string content = source_.substr(content_start_pos, end_pos - content_start_pos - 1);
        chunks_.push_back({ChunkType::Css, content});
        current_ = end_pos;
    } else {
        current_ = content_start_pos;
    }
}
void CHTLUnifiedScanner::handleChtlBlock() {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

} // namespace CHTL
