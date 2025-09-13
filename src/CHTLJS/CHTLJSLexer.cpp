#include "CHTLJSLexer.h"
#include <algorithm>

namespace CHTL {
namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source_(source) {}

std::vector<JSToken> CHTLJSLexer::scanTokens() {
    while (current_ < source_.length()) {
        findNextToken();
    }
    tokens_.push_back({JSTokenType::EndOfFile, ""});
    return tokens_;
}

void CHTLJSLexer::findNextToken() {
    // Find the earliest occurrence of any special token
    size_t selector_pos = source_.find("{{", current_);
    size_t listen_pos = source_.find("listen", current_);
    size_t scriptloader_pos = source_.find("scriptloader", current_);
    size_t arrow_pos = source_.find("->", current_);
    size_t dot_pos = source_.find(".", current_);

    size_t next_pos = -1;
    JSTokenType next_type = JSTokenType::StandardJS;

    auto update_next_pos = [&](size_t pos, JSTokenType type) {
        if (pos != std::string::npos && (next_pos == -1 || pos < next_pos)) {
            next_pos = pos;
            next_type = type;
        }
    };

    update_next_pos(selector_pos, JSTokenType::EnhancedSelector);
    update_next_pos(listen_pos, JSTokenType::ListenBlock);
    update_next_pos(scriptloader_pos, JSTokenType::ScriptLoaderBlock);
    update_next_pos(arrow_pos, JSTokenType::Arrow);
    // update_next_pos(dot_pos, JSTokenType::Arrow); // Temporarily removed due to conflicts with standard JS

    if (next_pos == -1) {
        // No more special tokens, the rest is standard JS
        if (current_ < source_.length()) {
            tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_)});
        }
        current_ = source_.length();
        return;
    }

    // We found a special token. Anything before it is standard JS.
    if (next_pos > current_) {
        tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_, next_pos - current_)});
    }

    current_ = next_pos;

    if (next_type == JSTokenType::EnhancedSelector) {
        size_t end_pos = source_.find("}}", current_ + 2);
        if (end_pos == std::string::npos) { tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_)}); current_ = source_.length(); }
        else { tokens_.push_back({JSTokenType::EnhancedSelector, source_.substr(current_ + 2, end_pos - (current_ + 2))}); current_ = end_pos + 2; }
    } else if (next_type == JSTokenType::ListenBlock || next_type == JSTokenType::ScriptLoaderBlock) {
        size_t keyword_len = (next_type == JSTokenType::ListenBlock) ? 6 : 12;
        size_t start_brace = source_.find('{', current_ + keyword_len);
        if (start_brace == std::string::npos) { tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_)}); current_ = source_.length(); return; }

        int brace_level = 1;
        size_t end_brace = std::string::npos;
        for (size_t i = start_brace + 1; i < source_.length(); ++i) {
            if (source_[i] == '{') brace_level++;
            else if (source_[i] == '}') {
                brace_level--;
                if (brace_level == 0) { end_brace = i; break; }
            }
        }

        if (end_brace == std::string::npos) { tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_)}); current_ = source_.length(); }
        else { tokens_.push_back({next_type, source_.substr(start_brace + 1, end_brace - (start_brace + 1))}); current_ = end_brace + 1; }
    } else if (next_type == JSTokenType::Arrow) {
        tokens_.push_back({JSTokenType::Arrow, ""});
        current_ += (source_[current_] == '-') ? 2 : 1;
    }
}

} // namespace CHTLJS
} // namespace CHTL
