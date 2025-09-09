#pragma once

#include "lexer/token.h"
#include "ast/ast_node.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace parser {

class Parser {
private:
    lexer::TokenStream tokens_;
    size_t current_token_index_;
    
    // Grammar rules
    std::unordered_map<lexer::TokenType, std::string> token_type_names_;
    std::unordered_set<lexer::TokenType> element_start_tokens_;
    std::unordered_set<lexer::TokenType> style_start_tokens_;
    std::unordered_set<lexer::TokenType> script_start_tokens_;
    
    void initialize_grammar_rules();
    
    // Token operations
    const lexer::Token& current_token() const;
    const lexer::Token& peek_token(size_t offset = 1) const;
    const lexer::Token& next_token();
    bool match_token(lexer::TokenType type) const;
    bool match_tokens(const std::vector<lexer::TokenType>& types) const;
    void consume_token(lexer::TokenType expected_type);
    void consume_tokens(const std::vector<lexer::TokenType>& expected_types);
    bool is_at_end() const;
    
    // Parsing methods
    ast::ASTNode::NodePtr parse_root();
    ast::ASTNode::NodePtr parse_element();
    ast::ASTNode::NodePtr parse_text();
    ast::ASTNode::NodePtr parse_style();
    ast::ASTNode::NodePtr parse_script();
    ast::ASTNode::NodePtr parse_template();
    ast::ASTNode::NodePtr parse_custom();
    ast::ASTNode::NodePtr parse_origin();
    ast::ASTNode::NodePtr parse_import();
    ast::ASTNode::NodePtr parse_configuration();
    ast::ASTNode::NodePtr parse_namespace();
    
    // Attribute parsing
    std::unordered_map<std::string, std::string> parse_attributes();
    std::string parse_attribute_value();
    
    // Style parsing
    std::string parse_style_content();
    std::string parse_style_rule();
    std::string parse_style_selector();
    std::string parse_style_property();
    std::string parse_style_value();
    
    // Script parsing
    std::string parse_script_content();
    
    // Template parsing
    ast::ASTNode::NodePtr parse_template_style();
    ast::ASTNode::NodePtr parse_template_element();
    ast::ASTNode::NodePtr parse_template_var();
    
    // Custom parsing
    ast::ASTNode::NodePtr parse_custom_style();
    ast::ASTNode::NodePtr parse_custom_element();
    ast::ASTNode::NodePtr parse_custom_var();
    
    // Origin parsing
    ast::ASTNode::NodePtr parse_origin_html();
    ast::ASTNode::NodePtr parse_origin_style();
    ast::ASTNode::NodePtr parse_origin_javascript();
    
    // Import parsing
    ast::ASTNode::NodePtr parse_import_html();
    ast::ASTNode::NodePtr parse_import_style();
    ast::ASTNode::NodePtr parse_import_javascript();
    ast::ASTNode::NodePtr parse_import_chtl();
    ast::ASTNode::NodePtr parse_import_cjmod();
    
    // Configuration parsing
    std::unordered_map<std::string, std::string> parse_configuration_values();
    
    // Namespace parsing
    std::string parse_namespace_name();
    
    // Utility methods
    bool is_element_start() const;
    bool is_style_start() const;
    bool is_script_start() const;
    bool is_template_start() const;
    bool is_custom_start() const;
    bool is_origin_start() const;
    bool is_import_start() const;
    bool is_configuration_start() const;
    bool is_namespace_start() const;
    
    std::string get_token_type_name(lexer::TokenType type) const;
    void error(const std::string& message) const;
    void error(const std::string& message, const lexer::Token& token) const;
    
public:
    explicit Parser(const lexer::TokenStream& tokens);
    
    ast::ASTNode::NodePtr parse();
    std::vector<ast::ASTNode::NodePtr> parse_multiple();
    
    void reset();
    size_t get_position() const { return current_token_index_; }
};

} // namespace parser
} // namespace chtl