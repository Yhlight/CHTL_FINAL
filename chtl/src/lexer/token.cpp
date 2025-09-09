#include "lexer/token.h"
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace lexer {

std::string Token::to_string() const {
    std::ostringstream oss;
    oss << "Token(type=" << static_cast<int>(type) 
        << ", value=\"" << value << "\""
        << ", line=" << line 
        << ", column=" << column 
        << ", position=" << position << ")";
    return oss.str();
}

bool Token::is_keyword() const {
    static const std::unordered_set<TokenType> keyword_types = {
        TokenType::TEXT,
        TokenType::STYLE,
        TokenType::SCRIPT,
        TokenType::TEMPLATE,
        TokenType::CUSTOM,
        TokenType::ORIGIN,
        TokenType::IMPORT,
        TokenType::NAMESPACE,
        TokenType::CONFIGURATION,
        TokenType::USE,
        TokenType::EXCEPT,
        TokenType::DELETE,
        TokenType::INSERT,
        TokenType::INHERIT,
        TokenType::FROM,
        TokenType::AS,
        TokenType::AT_TOP,
        TokenType::AT_BOTTOM,
        TokenType::AFTER,
        TokenType::BEFORE,
        TokenType::REPLACE,
        TokenType::TEMPLATE_STYLE,
        TokenType::TEMPLATE_ELEMENT,
        TokenType::TEMPLATE_VAR,
        TokenType::CUSTOM_STYLE,
        TokenType::CUSTOM_ELEMENT,
        TokenType::CUSTOM_VAR,
        TokenType::ORIGIN_HTML,
        TokenType::ORIGIN_STYLE,
        TokenType::ORIGIN_JAVASCRIPT,
        TokenType::IMPORT_HTML,
        TokenType::IMPORT_STYLE,
        TokenType::IMPORT_JAVASCRIPT,
        TokenType::IMPORT_CHTL,
        TokenType::IMPORT_CJMOD
    };
    return keyword_types.find(type) != keyword_types.end();
}

bool Token::is_operator() const {
    static const std::unordered_set<TokenType> operator_types = {
        TokenType::ASSIGN,
        TokenType::COLON,
        TokenType::SEMICOLON,
        TokenType::COMMA,
        TokenType::DOT,
        TokenType::ARROW,
        TokenType::PLUS,
        TokenType::MINUS,
        TokenType::MULTIPLY,
        TokenType::DIVIDE,
        TokenType::MODULO,
        TokenType::POWER,
        TokenType::EQUAL,
        TokenType::NOT_EQUAL,
        TokenType::LESS,
        TokenType::LESS_EQUAL,
        TokenType::GREATER,
        TokenType::GREATER_EQUAL,
        TokenType::AND,
        TokenType::OR,
        TokenType::NOT,
        TokenType::QUESTION,
        TokenType::COLON_TERNARY
    };
    return operator_types.find(type) != operator_types.end();
}

bool Token::is_literal() const {
    static const std::unordered_set<TokenType> literal_types = {
        TokenType::STRING_LITERAL,
        TokenType::NUMBER_LITERAL,
        TokenType::BOOLEAN_LITERAL
    };
    return literal_types.find(type) != literal_types.end();
}

bool Token::is_comment() const {
    static const std::unordered_set<TokenType> comment_types = {
        TokenType::SINGLE_LINE_COMMENT,
        TokenType::MULTI_LINE_COMMENT,
        TokenType::GENERATOR_COMMENT
    };
    return comment_types.find(type) != comment_types.end();
}

const Token& TokenStream::current() const {
    if (current_index_ >= tokens_.size()) {
        static const Token eof_token(TokenType::EOF_TOKEN);
        return eof_token;
    }
    return tokens_[current_index_];
}

const Token& TokenStream::peek(size_t offset) const {
    size_t index = current_index_ + offset;
    if (index >= tokens_.size()) {
        static const Token eof_token(TokenType::EOF_TOKEN);
        return eof_token;
    }
    return tokens_[index];
}

const Token& TokenStream::next() {
    if (current_index_ < tokens_.size()) {
        current_index_++;
    }
    return current();
}

bool TokenStream::has_next() const {
    return current_index_ < tokens_.size() - 1;
}

bool TokenStream::has_peek(size_t offset) const {
    return current_index_ + offset < tokens_.size();
}

void TokenStream::reset() {
    current_index_ = 0;
}

void TokenStream::add_token(const Token& token) {
    tokens_.push_back(token);
}

void TokenStream::add_tokens(const std::vector<Token>& tokens) {
    tokens_.insert(tokens_.end(), tokens.begin(), tokens.end());
}

} // namespace lexer
} // namespace chtl