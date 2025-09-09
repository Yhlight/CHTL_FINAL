#pragma once

#include "token.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace lexer {

class Lexer {
private:
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    size_t start_position_;
    
    // Keyword mapping
    std::unordered_map<std::string, TokenType> keywords_;
    std::unordered_map<std::string, TokenType> operators_;
    
    void initialize_keywords();
    void initialize_operators();
    
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
    Token scan_unknown();
    
    bool is_letter(char c) const;
    bool is_digit(char c) const;
    bool is_whitespace(char c) const;
    bool is_operator_start(char c) const;
    bool is_bracket(char c) const;
    
    Token create_token(TokenType type, const std::string& value);
    Token create_token(TokenType type);
    
public:
    explicit Lexer(const std::string& source);
    
    TokenStream tokenize();
    std::vector<Token> tokenize_to_vector();
    
    void reset();
    size_t get_position() const { return position_; }
    size_t get_line() const { return line_; }
    size_t get_column() const { return column_; }
};

} // namespace lexer
} // namespace chtl