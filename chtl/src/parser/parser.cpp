#include "parser/parser.h"
#include <stdexcept>
#include <sstream>

namespace chtl {
namespace parser {

Parser::Parser(const lexer::TokenStream& tokens) 
    : tokens_(tokens), current_token_index_(0) {
    initialize_grammar_rules();
}

void Parser::initialize_grammar_rules() {
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

const lexer::Token& Parser::current_token() const {
    return tokens_.current();
}

const lexer::Token& Parser::peek_token(size_t offset) const {
    return tokens_.peek(offset);
}

const lexer::Token& Parser::next_token() {
    return tokens_.next();
}

bool Parser::match_token(lexer::TokenType type) const {
    return current_token().type == type;
}

bool Parser::match_tokens(const std::vector<lexer::TokenType>& types) const {
    for (size_t i = 0; i < types.size(); ++i) {
        if (peek_token(i).type != types[i]) {
            return false;
        }
    }
    return true;
}

void Parser::consume_token(lexer::TokenType expected_type) {
    if (current_token().type != expected_type) {
        error("Expected " + get_token_type_name(expected_type) + 
              ", got " + get_token_type_name(current_token().type));
    }
    next_token();
}

void Parser::consume_tokens(const std::vector<lexer::TokenType>& expected_types) {
    for (auto type : expected_types) {
        consume_token(type);
    }
}

bool Parser::is_at_end() const {
    return current_token().type == lexer::TokenType::EOF_TOKEN;
}

ast::ASTNode::NodePtr Parser::parse_root() {
    auto root = std::make_shared<ast::ASTNode>(ast::NodeType::ROOT, "root");
    
    while (!is_at_end()) {
        auto node = parse_element();
        if (node) {
            root->add_child(node);
        }
    }
    
    return root;
}

ast::ASTNode::NodePtr Parser::parse_element() {
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
    } else if (is_style_start()) {
        return parse_style();
    } else if (is_script_start()) {
        return parse_script();
    } else if (is_element_start()) {
        return parse_element();
    } else if (match_token(lexer::TokenType::TEXT)) {
        return parse_text();
    }
    
    // Skip unknown tokens
    next_token();
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_text() {
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

ast::ASTNode::NodePtr Parser::parse_style() {
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

ast::ASTNode::NodePtr Parser::parse_script() {
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

ast::ASTNode::NodePtr Parser::parse_template() {
    if (match_token(lexer::TokenType::TEMPLATE)) {
        next_token(); // consume 'template'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        if (match_token(lexer::TokenType::TEMPLATE_STYLE)) {
            return parse_template_style();
        } else if (match_token(lexer::TokenType::TEMPLATE_ELEMENT)) {
            return parse_template_element();
        } else if (match_token(lexer::TokenType::TEMPLATE_VAR)) {
            return parse_template_var();
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_custom() {
    if (match_token(lexer::TokenType::CUSTOM)) {
        next_token(); // consume 'custom'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        if (match_token(lexer::TokenType::CUSTOM_STYLE)) {
            return parse_custom_style();
        } else if (match_token(lexer::TokenType::CUSTOM_ELEMENT)) {
            return parse_custom_element();
        } else if (match_token(lexer::TokenType::CUSTOM_VAR)) {
            return parse_custom_var();
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_origin() {
    if (match_token(lexer::TokenType::ORIGIN)) {
        next_token(); // consume 'origin'
        
        if (match_token(lexer::TokenType::LEFT_BRACKET)) {
            next_token(); // consume '['
        }
        
        if (match_token(lexer::TokenType::ORIGIN_HTML)) {
            return parse_origin_html();
        } else if (match_token(lexer::TokenType::ORIGIN_STYLE)) {
            return parse_origin_style();
        } else if (match_token(lexer::TokenType::ORIGIN_JAVASCRIPT)) {
            return parse_origin_javascript();
        }
    }
    
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_import() {
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

ast::ASTNode::NodePtr Parser::parse_configuration() {
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

ast::ASTNode::NodePtr Parser::parse_namespace() {
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

std::unordered_map<std::string, std::string> Parser::parse_attributes() {
    std::unordered_map<std::string, std::string> attributes;
    
    while (!is_at_end() && !match_token(lexer::TokenType::LEFT_BRACE)) {
        if (match_token(lexer::TokenType::IDENTIFIER)) {
            std::string key = current_token().value;
            next_token();
            
            if (match_token(lexer::TokenType::COLON) || match_token(lexer::TokenType::ASSIGN)) {
                next_token(); // consume ':' or '='
                
                std::string value = parse_attribute_value();
                attributes[key] = value;
            }
        } else {
            next_token();
        }
    }
    
    return attributes;
}

std::string Parser::parse_attribute_value() {
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

std::string Parser::parse_style_content() {
    std::string content;
    
    while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
        content += current_token().value + " ";
        next_token();
    }
    
    return content;
}

std::string Parser::parse_script_content() {
    std::string content;
    
    while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
        content += current_token().value + " ";
        next_token();
    }
    
    return content;
}

ast::ASTNode::NodePtr Parser::parse_template_style() {
    // Implementation for template style parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_template_element() {
    // Implementation for template element parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_template_var() {
    // Implementation for template var parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_custom_style() {
    // Implementation for custom style parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_custom_element() {
    // Implementation for custom element parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_custom_var() {
    // Implementation for custom var parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_origin_html() {
    // Implementation for origin HTML parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_origin_style() {
    // Implementation for origin style parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_origin_javascript() {
    // Implementation for origin JavaScript parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_import_html() {
    // Implementation for import HTML parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_import_style() {
    // Implementation for import style parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_import_javascript() {
    // Implementation for import JavaScript parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_import_chtl() {
    // Implementation for import CHTL parsing
    return nullptr;
}

ast::ASTNode::NodePtr Parser::parse_import_cjmod() {
    // Implementation for import CJMOD parsing
    return nullptr;
}

std::unordered_map<std::string, std::string> Parser::parse_configuration_values() {
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

std::string Parser::parse_namespace_name() {
    if (match_token(lexer::TokenType::IDENTIFIER)) {
        std::string name = current_token().value;
        next_token();
        return name;
    }
    
    return "";
}

bool Parser::is_element_start() const {
    return element_start_tokens_.find(current_token().type) != element_start_tokens_.end();
}

bool Parser::is_style_start() const {
    return style_start_tokens_.find(current_token().type) != style_start_tokens_.end();
}

bool Parser::is_script_start() const {
    return script_start_tokens_.find(current_token().type) != script_start_tokens_.end();
}

bool Parser::is_template_start() const {
    return match_token(lexer::TokenType::TEMPLATE);
}

bool Parser::is_custom_start() const {
    return match_token(lexer::TokenType::CUSTOM);
}

bool Parser::is_origin_start() const {
    return match_token(lexer::TokenType::ORIGIN);
}

bool Parser::is_import_start() const {
    return match_token(lexer::TokenType::IMPORT);
}

bool Parser::is_configuration_start() const {
    return match_token(lexer::TokenType::CONFIGURATION);
}

bool Parser::is_namespace_start() const {
    return match_token(lexer::TokenType::NAMESPACE);
}

std::string Parser::get_token_type_name(lexer::TokenType type) const {
    auto it = token_type_names_.find(type);
    return it != token_type_names_.end() ? it->second : "unknown";
}

void Parser::error(const std::string& message) const {
    throw std::runtime_error("Parser error: " + message);
}

void Parser::error(const std::string& message, const lexer::Token& token) const {
    std::ostringstream oss;
    oss << "Parser error at line " << token.line 
        << ", column " << token.column << ": " << message;
    throw std::runtime_error(oss.str());
}

ast::ASTNode::NodePtr Parser::parse() {
    return parse_root();
}

std::vector<ast::ASTNode::NodePtr> Parser::parse_multiple() {
    std::vector<ast::ASTNode::NodePtr> nodes;
    
    while (!is_at_end()) {
        auto node = parse_element();
        if (node) {
            nodes.push_back(node);
        }
    }
    
    return nodes;
}

void Parser::reset() {
    current_token_index_ = 0;
}

} // namespace parser
} // namespace chtl