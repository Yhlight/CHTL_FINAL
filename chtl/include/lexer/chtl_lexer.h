#pragma once

#include "token.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace chtl {
namespace lexer {

class CHTLLexer {
private:
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    size_t start_position_;
    
    // Keyword mapping
    std::unordered_map<std::string, TokenType> keywords_;
    std::unordered_map<std::string, TokenType> operators_;
    std::unordered_set<std::string> html_elements_;
    std::unordered_set<std::string> chtl_js_keywords_;
    
    void initialize_keywords();
    void initialize_operators();
    void initialize_html_elements();
    void initialize_chtl_js_keywords();
    
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    void skip_comment();
    void skip_single_line_comment();
    void skip_multi_line_comment();
    void skip_generator_comment();
    
    Token scan_identifier();
    Token scan_string_literal();
    Token scan_number_literal();
    Token scan_operator();
    Token scan_bracket();
    Token scan_chtl_special();
    Token scan_selector();
    Token scan_unknown();
    
    bool is_letter(char c) const;
    bool is_digit(char c) const;
    bool is_whitespace(char c) const;
    bool is_operator_start(char c) const;
    bool is_bracket(char c) const;
    bool is_chtl_special_start() const;
    bool is_selector_start() const;
    
    Token create_token(TokenType type, const std::string& value);
    Token create_token(TokenType type);
    
    // CHTL specific parsing
    Token scan_curly_selector();
    Token scan_unquoted_literal();
    
public:
    explicit CHTLLexer(const std::string& source);
    
    TokenStream tokenize();
    std::vector<Token> tokenize_to_vector();
    
    void reset();
    size_t get_position() const { return position_; }
    size_t get_line() const { return line_; }
    size_t get_column() const { return column_; }
};

} // namespace lexer
} // namespace chtl