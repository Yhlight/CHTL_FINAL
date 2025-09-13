#include "CHTLJSLexer.h"

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
    size_t start_pos = source_.find("{{", current_);

    if (start_pos == std::string::npos) {
        // No more selectors, the rest is standard JS
        if (current_ < source_.length()) {
            tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_)});
        }
        current_ = source_.length();
        return;
    }

    size_t end_pos = source_.find("}}", start_pos + 2);
    if (end_pos == std::string::npos) {
        // Unterminated selector, treat as standard JS
        if (current_ < source_.length()) {
            tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_)});
        }
        current_ = source_.length();
        return;
    }

    // We found a selector. Anything before it is standard JS.
    if (start_pos > current_) {
        tokens_.push_back({JSTokenType::StandardJS, source_.substr(current_, start_pos - current_)});
    }

    // Add the selector token
    tokens_.push_back({JSTokenType::EnhancedSelector, source_.substr(start_pos + 2, end_pos - (start_pos + 2))});

    current_ = end_pos + 2;
}

} // namespace CHTLJS
} // namespace CHTL
