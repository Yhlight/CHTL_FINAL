#include "CHTLJSLexer.h"

namespace CHTL {
namespace CHTL_JS {

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source_(source) {}

std::vector<JSToken> CHTLJSLexer::scanTokens() {
    std::vector<JSToken> tokens;
    while (pos_ < source_.length()) {
        if (source_.substr(pos_, 2) == "{{") {
            size_t end_pos = source_.find("}}", pos_);
            if (end_pos != std::string::npos) {
                end_pos += 2; // include the closing braces
                tokens.push_back({JSTokenType::Selector, source_.substr(pos_, end_pos - pos_)});
                pos_ = end_pos;
            } else {
                tokens.push_back({JSTokenType::Unexpected, source_.substr(pos_)});
                pos_ = source_.length();
            }
        } else if (source_.substr(pos_, 2) == "__") {
            size_t end_pos = source_.find("__", pos_ + 2);
             if (end_pos != std::string::npos) {
                end_pos += 2; // include the closing underscores
                tokens.push_back({JSTokenType::JSCode, source_.substr(pos_, end_pos - pos_)});
                pos_ = end_pos;
            } else {
                tokens.push_back({JSTokenType::Unexpected, source_.substr(pos_)});
                pos_ = source_.length();
            }
        } else {
            // Should not happen with the current scanner, but as a fallback
             tokens.push_back({JSTokenType::Unexpected, source_.substr(pos_)});
             pos_ = source_.length();
        }
    }
    tokens.push_back({JSTokenType::EndOfFile, ""});
    return tokens;
}

} // namespace CHTL_JS
} // namespace CHTL
