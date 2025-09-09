#include "parser/chtl_parser.h"
#include <stdexcept>
#include <sstream>

namespace chtl {
namespace parser {

CHTLParser::CHTLParser(const lexer::TokenStream& tokens) 
    : tokens_(tokens), current_token_index_(0) {
    initialize_grammar_rules();
}

void CHTLParser::initialize_grammar_rules() {
    // Token type names for error messages
    token_type_names_ = {
        {lexer::TokenType::IDENTIFIER, "identifier"},
        {lexer::TokenType::STRING_LITERAL, "string literal"},
        {lexer::TokenType::NUMBER_LITERAL, "number literal"},
        {lexer::TokenType::BOOLEAN_LITERAL, "boolean literal"},
        {lexer::TokenType::ASSIGN, "="},
        {lexer::TokenType::COLON, ":"},
        {lexer::TokenType::SEMICOLON, ";"},
        {lexer::TokenType::COMMA, ","},
        {lexer::TokenType::DOT, "."},
        {lexer::TokenType::ARROW, "->"},
        {lexer::TokenType::LEFT_BRACE, "{"},
        {lexer::TokenType::RIGHT_BRACE, "}"},
        {lexer::TokenType::LEFT_BRACKET, "["},
        {lexer::TokenType::RIGHT_BRACKET, "]"},
        {lexer::TokenType::LEFT_PAREN, "("},
        {lexer::TokenType::RIGHT_PAREN, ")"},
        {lexer::TokenType::EOF_TOKEN, "end of file"}
    };
    
    // Element start tokens
    element_start_tokens_ = {
        lexer::TokenType::IDENTIFIER,
        lexer::TokenType::TEXT
    };
    
    // Style start tokens
    style_start_tokens_ = {
        lexer::TokenType::STYLE
    };
    
    // Script start tokens
    script_start_tokens_ = {
        lexer::TokenType::SCRIPT
    };
}

const lexer::Token& CHTLParser::current_token() const {
    return tokens_.current();
}

const lexer::Token& CHTLParser::peek_token(size_t offset) const {
    return tokens_.peek(offset);
}

const lexer::Token& CHTLParser::next_token() {
    return tokens_.next();
}

bool CHTLParser::match_token(lexer::TokenType type) const {
    return current_token().type == type;
}

bool CHTLParser::match_tokens(const std::vector<lexer::TokenType>& types) const {
    for (size_t i = 0; i < types.size(); ++i) {
        if (peek_token(i).type != types[i]) {
            return false;
        }
    }
    return true;
}

void CHTLParser::consume_token(lexer::TokenType expected_type) {
    if (current_token().type != expected_type) {
        error("Expected " + get_token_type_name(expected_type) + 
              ", got " + get_token_type_name(current_token().type));
    }
    next_token();
}

void CHTLParser::consume_tokens(const std::vector<lexer::TokenType>& expected_types) {
    for (auto type : expected_types) {
        consume_token(type);
    }
}

bool CHTLParser::is_at_end() const {
    return current_token().type == lexer::TokenType::EOF_TOKEN;
}

ast::ASTNode::NodePtr CHTLParser::parse_root() {
    auto root = std::make_shared<ast::ASTNode>(ast::NodeType::ROOT, "root");
    
    while (!is_at_end()) {
        auto node = parse_element();
        if (node) {
            root->add_child(node);
        }
    }
    
    return root;
}

ast::ASTNode::NodePtr CHTLParser::parse_element() {
    if (is_template_start()) {
        return parse_template();
    } else if (is_custom_start()) {
        return parse_custom();
    } else if (is_origin_start()) {
        return parse_origin();
    } else if (is_import_start()) {
        return parse_import();
    } else if (is_configuration_start()) {
        return parse_configuration();
    } else if (is_namespace_start()) {
        return parse_namespace();
    } else if (is_use_start()) {
        return parse_use();
    } else if (is_style_start()) {
        return parse_style();
    } else if (is_script_start()) {
        return parse_script();
    } else if (match_token(lexer::TokenType::TEXT)) {
        return parse_text();
    } else if (is_element_start()) {
        return parse_html_element();
    }
    
    // Skip unknown tokens
    next_token();
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_html_element() {
    if (match_token(lexer::TokenType::IDENTIFIER)) {
        std::string element_name = current_token().value;
        next_token(); // consume element name
        
        auto element = std::make_shared<ast::ElementNode>(element_name);
        
        // Parse attributes (before the opening brace)
        while (!is_at_end() && !match_token(lexer::TokenType::LEFT_BRACE)) {
            if (match_token(lexer::TokenType::IDENTIFIER)) {
                std::string key = current_token().value;
                next_token();
                
                if (match_token(lexer::TokenType::COLON) || match_token(lexer::TokenType::ASSIGN)) {
                    next_token(); // consume ':' or '='
                    
                    std::string value = parse_attribute_value();
                    element->set_attribute(key, value);
                    
                    // Skip semicolon if present
                    if (match_token(lexer::TokenType::SEMICOLON)) {
                        next_token();
                    }
                }
            } else {
                next_token();
            }
        }
        
        // Parse children
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                // Check if this is an attribute
                if (match_token(lexer::TokenType::IDENTIFIER) && 
                    peek_token().type == lexer::TokenType::COLON) {
                    // Parse attribute
                    std::string key = current_token().value;
                    next_token(); // consume key
                    next_token(); // consume ':'
                    
                    std::string value = parse_attribute_value();
                    element->set_attribute(key, value);
                    
                    // Skip semicolon if present
                    if (match_token(lexer::TokenType::SEMICOLON)) {
                        next_token();
                    }
                } else {
                    // Parse as child element
                    auto child = parse_element();
                    if (child) {
                        element->add_child(child);
                    }
                }
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
        }
        
        return element;
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_text() {
    if (match_token(lexer::TokenType::TEXT)) {
        next_token(); // consume 'text'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content;
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                content += current_token().value + " ";
                next_token();
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return std::make_shared<ast::TextNode>(content);
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_style() {
    if (match_token(lexer::TokenType::STYLE)) {
        next_token(); // consume 'style'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content = parse_style_content();
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return std::make_shared<ast::StyleNode>(content);
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_script() {
    if (match_token(lexer::TokenType::SCRIPT)) {
        next_token(); // consume 'script'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content = parse_script_content();
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return std::make_shared<ast::ScriptNode>(content);
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_template() {
    if (match_token(lexer::TokenType::TEMPLATE)) {
        next_token(); // consume 'template'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        // Check for @Style, @Element, @Var after the bracket
        if (match_token(lexer::TokenType::STRING_LITERAL)) {
            std::string type = current_token().value;
            next_token(); // consume the type
            
            if (type == "@Style") {
                return parse_template_style();
            } else if (type == "@Element") {
                return parse_template_element();
            } else if (type == "@Var") {
                return parse_template_var();
            }
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_custom() {
    if (match_token(lexer::TokenType::CUSTOM)) {
        next_token(); // consume 'custom'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        // Check for @Style, @Element, @Var after the bracket
        if (match_token(lexer::TokenType::STRING_LITERAL)) {
            std::string type = current_token().value;
            next_token(); // consume the type
            
            if (type == "@Style") {
                return parse_custom_style();
            } else if (type == "@Element") {
                return parse_custom_element();
            } else if (type == "@Var") {
                return parse_custom_var();
            }
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_origin() {
    if (match_token(lexer::TokenType::ORIGIN)) {
        next_token(); // consume 'origin'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        // Check for @Html, @Style, @JavaScript after the bracket
        if (match_token(lexer::TokenType::STRING_LITERAL)) {
            std::string type = current_token().value;
            next_token(); // consume the type
            
            if (type == "@Html") {
                return parse_origin_html();
            } else if (type == "@Style") {
                return parse_origin_style();
            } else if (type == "@JavaScript") {
                return parse_origin_javascript();
            }
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_import() {
    if (match_token(lexer::TokenType::IMPORT)) {
        next_token(); // consume 'import'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        if (match_token(lexer::TokenType::IMPORT_HTML)) {
            return parse_import_html();
        } else if (match_token(lexer::TokenType::IMPORT_STYLE)) {
            return parse_import_style();
        } else if (match_token(lexer::TokenType::IMPORT_JAVASCRIPT)) {
            return parse_import_javascript();
        } else if (match_token(lexer::TokenType::IMPORT_CHTL)) {
            return parse_import_chtl();
        } else if (match_token(lexer::TokenType::IMPORT_CJMOD)) {
            return parse_import_cjmod();
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_configuration() {
    if (match_token(lexer::TokenType::CONFIGURATION)) {
        next_token(); // consume 'configuration'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            auto config = std::make_shared<ast::ConfigurationNode>();
            config->config_values = parse_configuration_values();
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return config;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_namespace() {
    if (match_token(lexer::TokenType::NAMESPACE)) {
        next_token(); // consume 'namespace'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        std::string name = parse_namespace_name();
        return std::make_shared<ast::NamespaceNode>(name);
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_use() {
    if (match_token(lexer::TokenType::USE)) {
        next_token(); // consume 'use'
        
        if (match_token(lexer::TokenType::IDENTIFIER)) {
            std::string value = current_token().value;
            next_token();
            
            if (value == "html5") {
                return parse_use_html5();
            } else {
                return parse_use_config();
            }
        }
    }
    
    return nullptr;
}

std::unordered_map<std::string, std::string> CHTLParser::parse_attributes() {
    std::unordered_map<std::string, std::string> attributes;
    
    while (!is_at_end() && !match_token(lexer::TokenType::LEFT_BRACE)) {
        if (match_token(lexer::TokenType::IDENTIFIER)) {
            std::string key = current_token().value;
            next_token();
            
            if (match_token(lexer::TokenType::COLON) || match_token(lexer::TokenType::ASSIGN)) {
                next_token(); // consume ':' or '='
                
                std::string value = parse_attribute_value();
                attributes[key] = value;
                
                // Skip semicolon if present
                if (match_token(lexer::TokenType::SEMICOLON)) {
                    next_token();
                }
            }
        } else {
            next_token();
        }
    }
    
    return attributes;
}

std::string CHTLParser::parse_attribute_value() {
    if (match_token(lexer::TokenType::STRING_LITERAL)) {
        std::string value = current_token().value;
        next_token();
        return value;
    } else if (match_token(lexer::TokenType::IDENTIFIER)) {
        std::string value = current_token().value;
        next_token();
        return value;
    } else if (match_token(lexer::TokenType::NUMBER_LITERAL)) {
        std::string value = current_token().value;
        next_token();
        return value;
    }
    
    return "";
}

std::string CHTLParser::parse_unquoted_literal() {
    if (match_token(lexer::TokenType::IDENTIFIER)) {
        std::string value = current_token().value;
        next_token();
        return value;
    }
    
    return "";
}

std::string CHTLParser::parse_style_content() {
    std::string content;
    
    while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
        content += current_token().value + " ";
        next_token();
    }
    
    return content;
}

std::string CHTLParser::parse_script_content() {
    std::string content;
    
    while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
        content += current_token().value + " ";
        next_token();
    }
    
    return content;
}

// Template parsing implementations
ast::ASTNode::NodePtr CHTLParser::parse_template_style() {
    if (match_token(lexer::TokenType::TEMPLATE_STYLE)) {
        next_token(); // consume '@Style'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content = parse_style_content();
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            auto template_node = std::make_shared<ast::ASTNode>(ast::NodeType::TEMPLATE_STYLE, "template_style");
            template_node->set_attribute("content", content);
            return template_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_template_element() {
    if (match_token(lexer::TokenType::TEMPLATE_ELEMENT)) {
        next_token(); // consume '@Element'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            auto template_node = std::make_shared<ast::ASTNode>(ast::NodeType::TEMPLATE_ELEMENT, "template_element");
            
            // Parse template content (elements, attributes, etc.)
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                auto child = parse_element();
                if (child) {
                    template_node->add_child(child);
                }
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return template_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_template_var() {
    if (match_token(lexer::TokenType::TEMPLATE_VAR)) {
        next_token(); // consume '@Var'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            auto template_node = std::make_shared<ast::ASTNode>(ast::NodeType::TEMPLATE_VAR, "template_var");
            
            // Parse variable definitions
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                if (match_token(lexer::TokenType::IDENTIFIER)) {
                    std::string var_name = current_token().value;
                    next_token();
                    
                    if (match_token(lexer::TokenType::COLON) || match_token(lexer::TokenType::ASSIGN)) {
                        next_token(); // consume ':' or '='
                        
                        std::string var_value = parse_attribute_value();
                        template_node->set_attribute(var_name, var_value);
                        
                        // Skip semicolon if present
                        if (match_token(lexer::TokenType::SEMICOLON)) {
                            next_token();
                        }
                    }
                } else {
                    next_token();
                }
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return template_node;
        }
    }
    
    return nullptr;
}

// Custom parsing implementations
ast::ASTNode::NodePtr CHTLParser::parse_custom_style() {
    if (match_token(lexer::TokenType::CUSTOM_STYLE)) {
        next_token(); // consume '@Style'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content = parse_style_content();
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            auto custom_node = std::make_shared<ast::ASTNode>(ast::NodeType::CUSTOM_STYLE, "custom_style");
            custom_node->set_attribute("content", content);
            return custom_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_custom_element() {
    if (match_token(lexer::TokenType::CUSTOM_ELEMENT)) {
        next_token(); // consume '@Element'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            auto custom_node = std::make_shared<ast::ASTNode>(ast::NodeType::CUSTOM_ELEMENT, "custom_element");
            
            // Parse custom content
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                auto child = parse_element();
                if (child) {
                    custom_node->add_child(child);
                }
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return custom_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_custom_var() {
    if (match_token(lexer::TokenType::CUSTOM_VAR)) {
        next_token(); // consume '@Var'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            auto custom_node = std::make_shared<ast::ASTNode>(ast::NodeType::CUSTOM_VAR, "custom_var");
            
            // Parse custom variable definitions
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                if (match_token(lexer::TokenType::IDENTIFIER)) {
                    std::string var_name = current_token().value;
                    next_token();
                    
                    if (match_token(lexer::TokenType::COLON) || match_token(lexer::TokenType::ASSIGN)) {
                        next_token(); // consume ':' or '='
                        
                        std::string var_value = parse_attribute_value();
                        custom_node->set_attribute(var_name, var_value);
                        
                        // Skip semicolon if present
                        if (match_token(lexer::TokenType::SEMICOLON)) {
                            next_token();
                        }
                    }
                } else {
                    next_token();
                }
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            return custom_node;
        }
    }
    
    return nullptr;
}

// Origin parsing implementations
ast::ASTNode::NodePtr CHTLParser::parse_origin_html() {
    if (match_token(lexer::TokenType::ORIGIN_HTML)) {
        next_token(); // consume '@Html'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content;
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                content += current_token().value + " ";
                next_token();
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            auto origin_node = std::make_shared<ast::ASTNode>(ast::NodeType::ORIGIN_HTML, "origin_html");
            origin_node->set_attribute("content", content);
            return origin_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_origin_style() {
    if (match_token(lexer::TokenType::ORIGIN_STYLE)) {
        next_token(); // consume '@Style'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content;
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                content += current_token().value + " ";
                next_token();
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            auto origin_node = std::make_shared<ast::ASTNode>(ast::NodeType::ORIGIN_STYLE, "origin_style");
            origin_node->set_attribute("content", content);
            return origin_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_origin_javascript() {
    if (match_token(lexer::TokenType::ORIGIN_JAVASCRIPT)) {
        next_token(); // consume '@JavaScript'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content;
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                content += current_token().value + " ";
                next_token();
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            auto origin_node = std::make_shared<ast::ASTNode>(ast::NodeType::ORIGIN_JAVASCRIPT, "origin_javascript");
            origin_node->set_attribute("content", content);
            return origin_node;
        }
    }
    
    return nullptr;
}

// Import parsing implementations
ast::ASTNode::NodePtr CHTLParser::parse_import_html() {
    if (match_token(lexer::TokenType::IMPORT_HTML)) {
        next_token(); // consume '@Html'
        
        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            next_token(); // consume '{'
            
            std::string content;
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                content += current_token().value + " ";
                next_token();
            }
            
            if (match_token(lexer::TokenType::RIGHT_BRACE)) {
                next_token(); // consume '}'
            }
            
            auto import_node = std::make_shared<ast::ASTNode>(ast::NodeType::IMPORT_HTML, "import_html");
            import_node->set_attribute("content", content);
            return import_node;
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_import_style() {
    // Implementation for import style parsing
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_import_javascript() {
    // Implementation for import JavaScript parsing
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_import_chtl() {
    // Implementation for import CHTL parsing
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_import_cjmod() {
    // Implementation for import CJMOD parsing
    return nullptr;
}

std::unordered_map<std::string, std::string> CHTLParser::parse_configuration_values() {
    std::unordered_map<std::string, std::string> values;
    
    while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
        if (match_token(lexer::TokenType::IDENTIFIER)) {
            std::string key = current_token().value;
            next_token();
            
            if (match_token(lexer::TokenType::ASSIGN)) {
                next_token(); // consume '='
                
                if (match_token(lexer::TokenType::STRING_LITERAL) || 
                    match_token(lexer::TokenType::IDENTIFIER) ||
                    match_token(lexer::TokenType::NUMBER_LITERAL)) {
                    values[key] = current_token().value;
                    next_token();
                }
            }
        } else {
            next_token();
        }
    }
    
    return values;
}

std::string CHTLParser::parse_namespace_name() {
    if (match_token(lexer::TokenType::IDENTIFIER)) {
        std::string name = current_token().value;
        next_token();
        return name;
    }
    
    return "";
}

ast::ASTNode::NodePtr CHTLParser::parse_use_html5() {
    auto use_node = std::make_shared<ast::ASTNode>(ast::NodeType::USE_OPERATOR, "use");
    use_node->set_attribute("type", "html5");
    return use_node;
}

ast::ASTNode::NodePtr CHTLParser::parse_use_config() {
    auto use_node = std::make_shared<ast::ASTNode>(ast::NodeType::USE_OPERATOR, "use");
    use_node->set_attribute("type", "config");
    return use_node;
}

// Utility methods
bool CHTLParser::is_element_start() const {
    return element_start_tokens_.find(current_token().type) != element_start_tokens_.end();
}

bool CHTLParser::is_style_start() const {
    return style_start_tokens_.find(current_token().type) != style_start_tokens_.end();
}

bool CHTLParser::is_script_start() const {
    return script_start_tokens_.find(current_token().type) != script_start_tokens_.end();
}

bool CHTLParser::is_template_start() const {
    return match_token(lexer::TokenType::TEMPLATE);
}

bool CHTLParser::is_custom_start() const {
    return match_token(lexer::TokenType::CUSTOM);
}

bool CHTLParser::is_origin_start() const {
    return match_token(lexer::TokenType::ORIGIN);
}

bool CHTLParser::is_import_start() const {
    return match_token(lexer::TokenType::IMPORT);
}

bool CHTLParser::is_configuration_start() const {
    return match_token(lexer::TokenType::CONFIGURATION);
}

bool CHTLParser::is_namespace_start() const {
    return match_token(lexer::TokenType::NAMESPACE);
}

bool CHTLParser::is_use_start() const {
    return match_token(lexer::TokenType::USE);
}

std::string CHTLParser::get_token_type_name(lexer::TokenType type) const {
    auto it = token_type_names_.find(type);
    return it != token_type_names_.end() ? it->second : "unknown";
}

void CHTLParser::error(const std::string& message) const {
    throw std::runtime_error("Parser error: " + message);
}

void CHTLParser::error(const std::string& message, const lexer::Token& token) const {
    std::ostringstream oss;
    oss << "Parser error at line " << token.line 
        << ", column " << token.column << ": " << message;
    throw std::runtime_error(oss.str());
}

void CHTLParser::push_context(ast::NodeType type) {
    context_stack_.push(type);
}

void CHTLParser::pop_context() {
    if (!context_stack_.empty()) {
        context_stack_.pop();
    }
}

ast::NodeType CHTLParser::current_context() const {
    return context_stack_.empty() ? ast::NodeType::ROOT : context_stack_.top();
}

ast::ASTNode::NodePtr CHTLParser::parse() {
    return parse_root();
}

std::vector<ast::ASTNode::NodePtr> CHTLParser::parse_multiple() {
    std::vector<ast::ASTNode::NodePtr> nodes;
    
    while (!is_at_end()) {
        auto node = parse_element();
        if (node) {
            nodes.push_back(node);
        }
    }
    
    return nodes;
}

void CHTLParser::reset() {
    current_token_index_ = 0;
    while (!context_stack_.empty()) {
        context_stack_.pop();
    }
}

} // namespace parser
} // namespace chtl