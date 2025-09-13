#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <cctype>
#include <vector>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    current_ = 0;

    while (current_ < source_.length()) {
        size_t script_pos = source_.find("script", current_);
        size_t style_pos = source_.find("style", current_);

        size_t keyword_pos = std::string::npos;
        bool is_script = false;

        if (script_pos != std::string::npos && (style_pos == std::string::npos || script_pos < style_pos)) {
            keyword_pos = script_pos;
            is_script = true;
        } else {
            keyword_pos = style_pos;
            is_script = false;
        }

        if (keyword_pos == std::string::npos) {
            if (last_pos < source_.length()) {
                chunks_.push_back({ChunkType::CHTL, pending_chtl_prefix_ + source_.substr(last_pos)});
                pending_chtl_prefix_.clear();
            }
            break;
        }

        size_t brace_pos = source_.find('{', keyword_pos);
        if (brace_pos == std::string::npos) {
             if (last_pos < source_.length()) {
                chunks_.push_back({ChunkType::CHTL, pending_chtl_prefix_ + source_.substr(last_pos)});
                pending_chtl_prefix_.clear();
            }
            break;
        }

        std::string pre_content = source_.substr(last_pos, brace_pos - last_pos + 1);
        chunks_.push_back({ChunkType::CHTL, pending_chtl_prefix_ + pre_content});
        pending_chtl_prefix_.clear();

        current_ = brace_pos + 1;

        size_t closing_brace_pos = 0;
        if (is_script) {
            closing_brace_pos = handleScriptTag();
        } else {
            closing_brace_pos = handleStyleTag();
        }

        pending_chtl_prefix_ += "}";

        current_ = closing_brace_pos + 1;
        last_pos = current_;
    }
     if (!pending_chtl_prefix_.empty()) {
        chunks_.push_back({ChunkType::CHTL, pending_chtl_prefix_});
        pending_chtl_prefix_.clear();
    }
}

size_t CHTLUnifiedScanner::handleStyleTag() {
    int brace_level = 1;
    size_t content_start = current_;

    while (current_ < source_.length() && brace_level > 0) {
        if (source_[current_] == '{') brace_level++;
        if (source_[current_] == '}') brace_level--;
        current_++;
    }

    size_t closing_brace_pos = current_ - 1;
    std::string content = source_.substr(content_start, closing_brace_pos - content_start);
    chunks_.push_back({ChunkType::Css, content});
    return closing_brace_pos;
}

size_t CHTLUnifiedScanner::handleScriptTag() {
    int brace_level = 1;
    size_t block_content_start = current_;

    while (current_ < source_.length() && brace_level > 0) {
        if (source_[current_] == '{') brace_level++;
        if (source_[current_] == '}') brace_level--;
        current_++;
    }

    size_t closing_brace_pos = current_ - 1;
    const std::string script_content = source_.substr(block_content_start, closing_brace_pos - block_content_start);

    std::string final_content;
    std::string js_buffer;
    size_t script_cursor = 0;

    while(script_cursor < script_content.length()){
        bool is_chtl_js_construct = false;

        auto handle_construct = [&](size_t start, size_t end) {
            if (!js_buffer.empty()) {
                std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
                placeholder_map_[placeholder] = js_buffer;
                final_content += placeholder;
                js_buffer.clear();
            }
            final_content += script_content.substr(start, end - start);
            script_cursor = end;
            is_chtl_js_construct = true;
        };

        if (script_content.substr(script_cursor, 2) == "{{") {
            size_t construct_end = script_content.find("}}", script_cursor);
            if (construct_end != std::string::npos) {
                handle_construct(script_cursor, construct_end + 2);
            }
        }

        if (!is_chtl_js_construct) {
            const std::vector<std::string> keywords = {"listen", "animate", "delegate", "vir", "router"};
            for (const auto& keyword : keywords) {
                if (script_content.substr(script_cursor, keyword.length()) == keyword) {
                    char next_char = script_content[script_cursor + keyword.length()];
                    if (isspace(next_char) || next_char == '{') {
                         size_t brace_start = script_content.find('{', script_cursor + keyword.length());
                         if (brace_start != std::string::npos) {
                            int inner_brace_level = 1;
                            size_t brace_end = brace_start + 1;
                            while(brace_end < script_content.length() && inner_brace_level > 0) {
                                if (script_content[brace_end] == '{') inner_brace_level++;
                                if (script_content[brace_end] == '}') inner_brace_level--;
                                brace_end++;
                            }
                            if (inner_brace_level == 0) {
                                handle_construct(script_cursor, brace_end);
                                break;
                            }
                         }
                    }
                }
            }
        }

        if (!is_chtl_js_construct) {
            js_buffer += script_content[script_cursor];
            script_cursor++;
        }
    }

    if (!js_buffer.empty()) {
        std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
        placeholder_map_[placeholder] = js_buffer;
        final_content += placeholder;
    }

    chunks_.push_back({ChunkType::ChtlJs, final_content});
    return closing_brace_pos;
}


std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

} // namespace CHTL
