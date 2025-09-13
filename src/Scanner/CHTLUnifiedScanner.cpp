#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>
#include <sstream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    chunks_.clear();
    placeholder_map_.clear();
    placeholder_id_ = 0;
    current_ = 0;

    std::stringstream chtl_builder;
    std::vector<CodeChunk> cjjs_chunks;

    size_t last_pos = 0;

    while (current_ < source_.length()) {
        if (source_[current_] == '<') {
            if (current_ + 7 < source_.length() && source_.substr(current_, 7) == "<script") {
                chtl_builder << source_.substr(last_pos, current_ - last_pos);
                handleScriptTag(chtl_builder);
                last_pos = current_;
                continue;
            } else if (current_ + 6 < source_.length() && source_.substr(current_, 6) == "<style") {
                chtl_builder << source_.substr(last_pos, current_ - last_pos);
                handleStyleTag(chtl_builder);
                last_pos = current_;
                continue;
            }
        } else if (source_.substr(current_, 8) == "[Origin]") {
            chtl_builder << source_.substr(last_pos, current_ - last_pos);
            handleOriginBlock(chtl_builder);
            last_pos = current_;
            continue;
        } else if (source_.substr(current_, 6) == "script" && (current_ + 6 < source_.length() && (isspace(source_[current_ + 6]) || source_[current_ + 6] == '{'))) {
            chtl_builder << source_.substr(last_pos, current_ - last_pos);
            cjjs_chunks.push_back(handleChtlBlock());
            last_pos = current_;
            continue;
        }
        current_++;
    }

    if (last_pos < source_.length()) {
        chtl_builder << source_.substr(last_pos);
    }

    std::vector<CodeChunk> final_chunks;
    if (chtl_builder.str().length() > 0) {
        final_chunks.push_back({ChunkType::CHTL, chtl_builder.str()});
    }
    final_chunks.insert(final_chunks.end(), cjjs_chunks.begin(), cjjs_chunks.end());

    return final_chunks;
}

void CHTLUnifiedScanner::process() {}

void CHTLUnifiedScanner::handleScriptTag(std::stringstream& builder) {
    size_t tag_open_end = source_.find('>', current_ + 1);
    if (tag_open_end == std::string::npos) {
        current_ = source_.length(); return;
    }
    size_t script_end_tag_pos = source_.find("</script>", tag_open_end);
    if (script_end_tag_pos == std::string::npos) {
        current_ = source_.length(); return;
    }
    size_t content_start = tag_open_end + 1;
    std::string content = source_.substr(content_start, script_end_tag_pos - content_start);
    std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    placeholder_map_[placeholder] = content;
    builder << source_.substr(current_, tag_open_end - current_ + 1) << placeholder << "</script>";
    current_ = script_end_tag_pos + 9;
}

void CHTLUnifiedScanner::handleStyleTag(std::stringstream& builder) {
    size_t tag_open_end = source_.find('>', current_ + 1);
    if (tag_open_end == std::string::npos) {
        current_ = source_.length(); return;
    }
    size_t style_end_tag_pos = source_.find("</style>", tag_open_end);
    if (style_end_tag_pos == std::string::npos) {
        current_ = source_.length(); return;
    }
    size_t content_start = tag_open_end + 1;
    std::string content = source_.substr(content_start, style_end_tag_pos - content_start);
    std::string placeholder = "__CSS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    placeholder_map_[placeholder] = content;
    builder << source_.substr(current_, tag_open_end - current_ + 1) << placeholder << "</style>";
    current_ = style_end_tag_pos + 8;
}

CodeChunk CHTLUnifiedScanner::handleChtlBlock() {
    size_t keyword_end = current_ + 6;
    size_t brace_open_pos = source_.find('{', keyword_end);
    if (brace_open_pos != std::string::npos) {
        bool only_whitespace = true;
        for (size_t i = keyword_end; i < brace_open_pos; ++i) {
            if (!isspace(source_[i])) { only_whitespace = false; break; }
        }
        if (only_whitespace) {
            int brace_level = 1;
            size_t content_start = brace_open_pos + 1;
            size_t content_end = content_start;
            while (content_end < source_.length() && brace_level > 0) {
                if (source_[content_end] == '{') brace_level++;
                else if (source_[content_end] == '}') brace_level--;
                content_end++;
            }
            if (brace_level == 0) {
                std::string content = source_.substr(content_start, content_end - content_start - 1);
                current_ = content_end;
                return {ChunkType::ChtlJs, content};
            }
        }
    }
    current_ += 6;
    return {ChunkType::ChtlJs, ""};
}

void CHTLUnifiedScanner::handleOriginBlock(std::stringstream& builder) {
    size_t tag_end = source_.find('{', current_);
    if (tag_end == std::string::npos) {
        current_ += 8; return;
    }
    std::string tag_header = source_.substr(current_, tag_end - current_);
    if (tag_header.find("@JavaScript") != std::string::npos) {
        int brace_level = 1;
        size_t content_start = tag_end + 1;
        size_t content_end = content_start;
        while (content_end < source_.length() && brace_level > 0) {
            if (source_[content_end] == '{') brace_level++;
            else if (source_[content_end] == '}') brace_level--;
            content_end++;
        }
        if (brace_level == 0) {
            std::string content = source_.substr(content_start, content_end - content_start - 1);
            std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
            placeholder_map_[placeholder] = content;
            builder << source_.substr(current_, tag_end - current_ + 1) << placeholder << "}";
            current_ = content_end;
            return;
        }
    }
    builder << source_.substr(current_, tag_end + 1 - current_);
    current_ = tag_end + 1;
}

} // namespace CHTL
