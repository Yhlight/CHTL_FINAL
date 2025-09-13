#include "CHTLJSLexer.h"
#include <cctype>

namespace CHTL {
namespace CHTL_JS {

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source_(source) {}

std::vector<JSToken> CHTLJSLexer::scanTokens() {
    std::vector<JSToken> tokens;
    while (pos_ < source_.length()) {
        char c = source_[pos_];
        if (isspace(c)) {
            pos_++;
            continue;
        }

        if (source_.substr(pos_, 2) == "{{") {
            size_t start = pos_;
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
            size_t start = pos_;
            size_t end_pos = source_.find("__", pos_ + 2);
             if (end_pos != std::string::npos) {
                end_pos += 2; // include the closing underscores
                tokens.push_back({JSTokenType::JSCode, source_.substr(pos_, end_pos - pos_)});
                pos_ = end_pos;
            } else {
                tokens.push_back({JSTokenType::Unexpected, source_.substr(pos_)});
                pos_ = source_.length();
            }
        } else if (source_.substr(pos_, 2) == "->") {
            tokens.push_back({JSTokenType::Arrow, "->"});
            pos_ += 2;
        } else if (isalpha(c) || c == '_') {
            size_t start = pos_;
            while (pos_ < source_.length() && (isalnum(source_[pos_]) || source_[pos_] == '_')) {
                pos_++;
            }
            std::string text = source_.substr(start, pos_ - start);
            if (text == "listen") {
                tokens.push_back({JSTokenType::Listen, text});
            } else {
                tokens.push_back({JSTokenType::Identifier, text});
            }
        } else if (c == '{') {
            tokens.push_back({JSTokenType::OpenBrace, "{"});
            pos_++;
        } else if (c == '}') {
            tokens.push_back({JSTokenType::CloseBrace, "}"});
            pos_++;
        } else if (c == ':') {
            tokens.push_back({JSTokenType::Colon, ":"});
            pos_++;
        } else if (c == ',') {
            tokens.push_back({JSTokenType::Comma, ","});
            pos_++;
        }
        else {
             tokens.push_back({JSTokenType::Unexpected, std::string(1, source_[pos_])});
             pos_++;
        }
    }
    tokens.push_back({JSTokenType::EndOfFile, ""});
    return tokens;
}

} // namespace CHTL_JS
} // namespace CHTL
