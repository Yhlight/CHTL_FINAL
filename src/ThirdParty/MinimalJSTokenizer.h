#ifndef MINIMAL_JS_TOKENIZER_H
#define MINIMAL_JS_TOKENIZER_H

#include <string>
#include <vector>
#include <unordered_set>

namespace CHTL {
namespace ThirdParty {

enum class JSTokenType {
    Unknown,
    Keyword,
    Identifier,
    StringLiteral,
    NumberLiteral,
    Operator,
    Whitespace,
    CHTLLiteral, // For {{...}}
};

struct JSToken {
    JSTokenType type;
    std::string text;
};

class MinimalJSTokenizer {
public:
    explicit MinimalJSTokenizer(const std::string& source) : source_(source), pos_(0) {}

    std::vector<JSToken> tokenize() {
        std::vector<JSToken> tokens;
        while (pos_ < source_.length()) {
            char current_char = source_[pos_];
            if (isspace(current_char)) {
                tokens.push_back(consumeWhitespace());
            } else if (current_char == '"' || current_char == '\'') {
                tokens.push_back(consumeStringLiteral(current_char));
            } else if (isdigit(current_char)) {
                tokens.push_back(consumeNumberLiteral());
            } else if (isalpha(current_char) || current_char == '_') {
                tokens.push_back(consumeIdentifierOrKeyword());
            } else if (source_.substr(pos_, 2) == "{{") {
                 tokens.push_back(consumeCHTLLiteral());
            }
            else {
                tokens.push_back(consumeOperator());
            }
        }
        return tokens;
    }

private:
    const std::string& source_;
    size_t pos_;
    const std::unordered_set<std::string> keywords_ = {
        "const", "let", "var", "function", "if", "else", "for", "while",
        "return", "class", "new", "this", "true", "false", "null",
        "listen" // Add listen as a keyword
    };

    JSToken consumeWhitespace() {
        size_t start = pos_;
        while (pos_ < source_.length() && isspace(source_[pos_])) {
            pos_++;
        }
        return {JSTokenType::Whitespace, source_.substr(start, pos_ - start)};
    }

    JSToken consumeStringLiteral(char quote) {
        size_t start = pos_;
        pos_++; // consume opening quote
        while (pos_ < source_.length()) {
            if (source_[pos_] == '\\') { // handle escaped characters
                pos_ += 2;
                continue;
            }
            if (source_[pos_] == quote) {
                pos_++; // consume closing quote
                break;
            }
            pos_++;
        }
        return {JSTokenType::StringLiteral, source_.substr(start, pos_ - start)};
    }

    JSToken consumeNumberLiteral() {
        size_t start = pos_;
        while (pos_ < source_.length() && isdigit(source_[pos_])) {
            pos_++;
        }
        return {JSTokenType::NumberLiteral, source_.substr(start, pos_ - start)};
    }

    JSToken consumeIdentifierOrKeyword() {
        size_t start = pos_;
        while (pos_ < source_.length() && (isalnum(source_[pos_]) || source_[pos_] == '_')) {
            pos_++;
        }
        std::string text = source_.substr(start, pos_ - start);
        JSTokenType type = keywords_.count(text) ? JSTokenType::Keyword : JSTokenType::Identifier;
        return {type, text};
    }

    JSToken consumeCHTLLiteral() {
        size_t start = pos_;
        size_t end = source_.find("}}", start);
        if (end != std::string::npos) {
            pos_ = end + 2;
            return {JSTokenType::CHTLLiteral, source_.substr(start, pos_ - start)};
        }
        // Fallback if no closing braces
        pos_ = source_.length();
        return {JSTokenType::Unknown, source_.substr(start)};
    }

    JSToken consumeOperator() {
        size_t start = pos_;
        if (source_.substr(pos_, 2) == "->") {
            pos_ += 2;
            return {JSTokenType::Operator, "->"};
        }

        pos_++; // Consume at least one char for other single-char operators
        return {JSTokenType::Operator, source_.substr(start, 1)};
    }
};

} // namespace ThirdParty
} // namespace CHTL

#endif // MINIMAL_JS_TOKENIZER_H
