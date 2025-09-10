#pragma once

#include "lexer/token.h"
#include "ast/ast_node.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <stack>

namespace chtl {
namespace parser {

class CHTLParser {
private:
    lexer::TokenStream tokens_;
    size_t current_token_index_;
    
    // Grammar rules
    std::unordered_map<lexer::TokenType, std::string> token_type_names_;
    std::unordered_set<lexer::TokenType> element_start_tokens_;
    std::unordered_set<lexer::TokenType> style_start_tokens_;
    std::unordered_set<lexer::TokenType> script_start_tokens_;
    
    // Context stack for nested parsing
    std::stack<ast::NodeType> context_stack_;
    
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
    ast::ASTNode::NodePtr parse_html_element();
    ast::ASTNode::NodePtr parse_text();
    ast::ASTNode::NodePtr parse_style();
    ast::ASTNode::NodePtr parse_script();
    ast::ASTNode::NodePtr parse_template();
    ast::ASTNode::NodePtr parse_custom();
    ast::ASTNode::NodePtr parse_origin();
    ast::ASTNode::NodePtr parse_import();
    ast::ASTNode::NodePtr parse_configuration();
    ast::ASTNode::NodePtr parse_namespace();
    ast::ASTNode::NodePtr parse_use();
    
    // Attribute parsing
    ast::ASTNode::AttributeMap parse_attributes();
    ast::ASTNode::NodePtr parse_attribute_value();

    // Expression parsing
    ast::ASTNode::NodePtr parse_expression();
    ast::ASTNode::NodePtr parse_conditional_expression();
    std::vector<std::string> parse_constraints();
    ast::ASTNode::NodePtr parse_logical_or_expression();
    ast::ASTNode::NodePtr parse_logical_and_expression();
    ast::ASTNode::NodePtr parse_equality_expression();
    ast::ASTNode::NodePtr parse_relational_expression();
    ast::ASTNode::NodePtr parse_additive_expression();
    ast::ASTNode::NodePtr parse_multiplicative_expression();
    ast::ASTNode::NodePtr parse_power_expression();
    ast::ASTNode::NodePtr parse_unary_expression();
    ast::ASTNode::NodePtr parse_primary_expression();
    
    // Style parsing
    std::string parse_style_content();
    std::string parse_style_rule();
    std::string parse_style_selector();
    std::string parse_style_property();
    std::string parse_style_value();
    std::string parse_style_condition_expression();
    std::string parse_style_property_reference();
    
    // Script parsing
    std::string parse_script_content();
    std::string parse_chtl_js_function();
    std::string parse_enhanced_selector();
    
    // Template parsing
    ast::ASTNode::NodePtr parse_template_style();
    ast::ASTNode::NodePtr parse_template_element(bool is_usage = false);
    ast::ASTNode::NodePtr parse_template_var();
    
    // Custom parsing
    ast::ASTNode::NodePtr parse_custom_style();
    ast::ASTNode::NodePtr parse_custom_element(bool is_usage = false);
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
    ast::ASTNode::NodePtr parse_granular_import();
    
    // Configuration parsing
    std::unordered_map<std::string, std::string> parse_configuration_values();
    std::unordered_map<std::string, std::vector<std::string>> parse_name_configuration();
    
    // Namespace parsing
    std::string parse_namespace_name();
    
    // Use parsing
    ast::ASTNode::NodePtr parse_use_html5();
    ast::ASTNode::NodePtr parse_use_config();
    
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
    bool is_use_start() const;
    
    std::string get_token_type_name(lexer::TokenType type) const;
    void error(const std::string& message) const;
    void error(const std::string& message, const lexer::Token& token) const;
    
    // Context management
    void push_context(ast::NodeType type);
    void pop_context();
    ast::NodeType current_context() const;
    
public:
    explicit CHTLParser(const lexer::TokenStream& tokens);
    
    ast::ASTNode::NodePtr parse();
    std::vector<ast::ASTNode::NodePtr> parse_multiple();
    
    void reset();
    size_t get_position() const { return current_token_index_; }
};

} // namespace parser
} // namespace chtl