#include "chtl_js/chtl_js_lexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace chtl {
namespace chtl_js {

CHTLJSLexer::CHTLJSLexer(const std::string& source) 
    : source_(source), position_(0), line_(1), column_(1), start_(0) {
    initialize_keywords();
}

void CHTLJSLexer::initialize_keywords() {
    // CHTL JS特有关键字
    keywords_["fileloader"] = CHTLJSTokenType::FILELOADER;
    keywords_["listen"] = CHTLJSTokenType::LISTEN;
    keywords_["animate"] = CHTLJSTokenType::ANIMATE;
    keywords_["vir"] = CHTLJSTokenType::VIR;
    keywords_["router"] = CHTLJSTokenType::ROUTER;
    keywords_["iNeverAway"] = CHTLJSTokenType::INEVERAWAY;
    keywords_["util"] = CHTLJSTokenType::UTIL;
    keywords_["change"] = CHTLJSTokenType::CHANGE;
    keywords_["then"] = CHTLJSTokenType::THEN;
    
    // 控制流关键字
    keywords_["if"] = CHTLJSTokenType::IF;
    keywords_["else"] = CHTLJSTokenType::ELSE;
    keywords_["for"] = CHTLJSTokenType::FOR;
    keywords_["while"] = CHTLJSTokenType::WHILE;
    keywords_["switch"] = CHTLJSTokenType::SWITCH;
    keywords_["case"] = CHTLJSTokenType::CASE;
    keywords_["default"] = CHTLJSTokenType::DEFAULT;
    keywords_["break"] = CHTLJSTokenType::BREAK;
    keywords_["continue"] = CHTLJSTokenType::CONTINUE;
    keywords_["return"] = CHTLJSTokenType::RETURN;
    
    // 函数和类关键字
    keywords_["function"] = CHTLJSTokenType::FUNCTION;
    keywords_["class"] = CHTLJSTokenType::CLASS;
    keywords_["new"] = CHTLJSTokenType::NEW;
    keywords_["this"] = CHTLJSTokenType::THIS;
    keywords_["super"] = CHTLJSTokenType::SUPER;
    
    // 异常处理关键字
    keywords_["try"] = CHTLJSTokenType::TRY;
    keywords_["catch"] = CHTLJSTokenType::CATCH;
    keywords_["finally"] = CHTLJSTokenType::FINALLY;
    keywords_["throw"] = CHTLJSTokenType::THROW;
    
    // 变量声明关键字
    keywords_["var"] = CHTLJSTokenType::VAR;
    keywords_["let"] = CHTLJSTokenType::LET;
    keywords_["const"] = CHTLJSTokenType::CONST;
    keywords_["async"] = CHTLJSTokenType::ASYNC;
    keywords_["await"] = CHTLJSTokenType::AWAIT;
    keywords_["yield"] = CHTLJSTokenType::YIELD;
    
    // 布尔字面量
    keywords_["true"] = CHTLJSTokenType::BOOLEAN_LITERAL;
    keywords_["false"] = CHTLJSTokenType::BOOLEAN_LITERAL;
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    
    while (!is_at_end()) {
        start_ = position_;
        CHTLJSToken token = next_token();
        tokens.push_back(token);
        
        if (token.type == CHTLJSTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return tokens;
}

CHTLJSToken CHTLJSLexer::next_token() {
    skip_whitespace();
    
    if (is_at_end()) {
        return make_token(CHTLJSTokenType::EOF_TOKEN);
    }
    
    char c = current_char();
    
    // 标识符和关键字
    if (is_alpha(c) || c == '_') {
        return scan_identifier();
    }
    
    // 数字
    if (is_digit(c)) {
        return scan_number();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return scan_string();
    }
    
    // 增强选择器 {{ }}
    if (c == '{' && peek_char() == '{') {
        return scan_selector();
    }
    
    // 运算符和分隔符
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || 
        c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|') {
        return scan_operator();
    }
    
    // 分隔符
    switch (c) {
        case '(': advance(); return make_token(CHTLJSTokenType::LEFT_PAREN);
        case ')': advance(); return make_token(CHTLJSTokenType::RIGHT_PAREN);
        case '{': advance(); return make_token(CHTLJSTokenType::LEFT_BRACE);
        case '}': advance(); return make_token(CHTLJSTokenType::RIGHT_BRACE);
        case '[': advance(); return make_token(CHTLJSTokenType::LEFT_BRACKET);
        case ']': advance(); return make_token(CHTLJSTokenType::RIGHT_BRACKET);
        case ';': advance(); return make_token(CHTLJSTokenType::SEMICOLON);
        case ',': advance(); return make_token(CHTLJSTokenType::COMMA);
        case '.': advance(); return make_token(CHTLJSTokenType::DOT);
        case ':': advance(); return make_token(CHTLJSTokenType::COLON);
        case '?': advance(); return make_token(CHTLJSTokenType::QUESTION);
        default:
            advance();
            return make_token(CHTLJSTokenType::UNKNOWN, std::string(1, c));
    }
}

CHTLJSToken CHTLJSLexer::peek_token(size_t offset) {
    size_t saved_position = position_;
    size_t saved_line = line_;
    size_t saved_column = column_;
    size_t saved_start = start_;
    
    CHTLJSToken token = next_token();
    
    position_ = saved_position;
    line_ = saved_line;
    column_ = saved_column;
    start_ = saved_start;
    
    return token;
}

bool CHTLJSLexer::is_at_end() const {
    return position_ >= source_.length();
}

size_t CHTLJSLexer::get_position() const {
    return position_;
}

size_t CHTLJSLexer::get_line() const {
    return line_;
}

size_t CHTLJSLexer::get_column() const {
    return column_;
}

void CHTLJSLexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    start_ = 0;
}

char CHTLJSLexer::current_char() const {
    if (is_at_end()) return '\0';
    return source_[position_];
}

char CHTLJSLexer::peek_char(size_t offset) const {
    if (position_ + offset >= source_.length()) return '\0';
    return source_[position_ + offset];
}

void CHTLJSLexer::advance() {
    if (!is_at_end()) {
        if (current_char() == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLJSLexer::skip_whitespace() {
    while (!is_at_end() && is_whitespace(current_char())) {
        if (current_char() == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLJSLexer::skip_comment() {
    if (current_char() == '/' && peek_char() == '/') {
        skip_until_newline();
    } else if (current_char() == '/' && peek_char() == '*') {
        advance(); // 跳过 /
        advance(); // 跳过 *
        
        while (!is_at_end()) {
            if (current_char() == '*' && peek_char() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                break;
            }
            advance();
        }
    }
}

bool CHTLJSLexer::is_alpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::is_digit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::is_alnum(char c) const {
    return is_alpha(c) || is_digit(c);
}

bool CHTLJSLexer::is_whitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

CHTLJSToken CHTLJSLexer::scan_identifier() {
    while (is_alnum(current_char())) {
        advance();
    }
    
    std::string lexeme = get_lexeme();
    
    // 检查是否是关键字
    auto it = keywords_.find(lexeme);
    if (it != keywords_.end()) {
        return make_token(it->second, lexeme);
    }
    
    return make_token(CHTLJSTokenType::IDENTIFIER, lexeme);
}

CHTLJSToken CHTLJSLexer::scan_string() {
    char quote = current_char();
    advance(); // 跳过开始引号
    
    while (!is_at_end() && current_char() != quote) {
        if (current_char() == '\\') {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    if (!is_at_end()) {
        advance(); // 跳过结束引号
    }
    
    return make_token(CHTLJSTokenType::STRING_LITERAL, get_lexeme());
}

CHTLJSToken CHTLJSLexer::scan_number() {
    while (is_digit(current_char())) {
        advance();
    }
    
    // 处理小数
    if (current_char() == '.' && is_digit(peek_char())) {
        advance(); // 跳过小数点
        while (is_digit(current_char())) {
            advance();
        }
    }
    
    // 处理科学计数法
    if (current_char() == 'e' || current_char() == 'E') {
        advance(); // 跳过 e 或 E
        if (current_char() == '+' || current_char() == '-') {
            advance(); // 跳过符号
        }
        while (is_digit(current_char())) {
            advance();
        }
    }
    
    return make_token(CHTLJSTokenType::NUMBER_LITERAL, get_lexeme());
}

CHTLJSToken CHTLJSLexer::scan_operator() {
    char c = current_char();
    advance();
    
    switch (c) {
        case '+':
            return make_token(CHTLJSTokenType::PLUS);
        case '-':
            if (current_char() == '>') {
                advance();
                return make_token(CHTLJSTokenType::ARROW);
            }
            return make_token(CHTLJSTokenType::MINUS);
        case '*':
            if (current_char() == '*') {
                advance();
                return make_token(CHTLJSTokenType::POWER);
            }
            return make_token(CHTLJSTokenType::MULTIPLY);
        case '/':
            return make_token(CHTLJSTokenType::DIVIDE);
        case '%':
            return make_token(CHTLJSTokenType::MODULO);
        case '=':
            if (current_char() == '=') {
                advance();
                return make_token(CHTLJSTokenType::EQUAL);
            }
            return make_token(CHTLJSTokenType::ASSIGN);
        case '!':
            if (current_char() == '=') {
                advance();
                return make_token(CHTLJSTokenType::NOT_EQUAL);
            }
            return make_token(CHTLJSTokenType::NOT);
        case '<':
            if (current_char() == '=') {
                advance();
                return make_token(CHTLJSTokenType::LESS_EQUAL);
            }
            return make_token(CHTLJSTokenType::LESS);
        case '>':
            if (current_char() == '=') {
                advance();
                return make_token(CHTLJSTokenType::GREATER_EQUAL);
            }
            return make_token(CHTLJSTokenType::GREATER);
        case '&':
            if (current_char() == '&') {
                advance();
                return make_token(CHTLJSTokenType::AND);
            }
            return make_token(CHTLJSTokenType::UNKNOWN, "&");
        case '|':
            if (current_char() == '|') {
                advance();
                return make_token(CHTLJSTokenType::OR);
            }
            return make_token(CHTLJSTokenType::UNKNOWN, "|");
        default:
            return make_token(CHTLJSTokenType::UNKNOWN, std::string(1, c));
    }
}

CHTLJSToken CHTLJSLexer::scan_selector() {
    // 跳过 {{
    advance();
    advance();
    
    while (!is_at_end() && !(current_char() == '}' && peek_char() == '}')) {
        advance();
    }
    
    if (!is_at_end()) {
        advance(); // 跳过 }
        advance(); // 跳过 }
    }
    
    std::string lexeme = get_lexeme();
    
    // 检查是否是自引用 {{&}}
    if (lexeme == "{{&}}") {
        return make_token(CHTLJSTokenType::SELF_REF, lexeme);
    }
    
    return make_token(CHTLJSTokenType::SELECTOR_START, lexeme);
}

bool CHTLJSLexer::match_string(const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (position_ + i >= source_.length() || source_[position_ + i] != str[i]) {
            return false;
        }
    }
    return true;
}

void CHTLJSLexer::skip_until_newline() {
    while (!is_at_end() && current_char() != '\n') {
        advance();
    }
}

std::string CHTLJSLexer::get_lexeme() const {
    return source_.substr(start_, position_ - start_);
}

CHTLJSToken CHTLJSLexer::make_token(CHTLJSTokenType type) {
    return make_token(type, get_lexeme());
}

CHTLJSToken CHTLJSLexer::make_token(CHTLJSTokenType type, const std::string& value) {
    return CHTLJSToken(type, value, line_, column_, start_);
}

} // namespace chtl_js
} // namespace chtl