#include "parser/chtl_parser.h"
#include "ast/ast_node.h"
#include <iostream>

namespace chtl {
namespace parser {

// NOTE: This is a partial, conceptual implementation focusing on the new logic.
// A full implementation would require all helper methods from the header to be defined.

CHTLParser::CHTLParser(const lexer::TokenStream& tokens) : tokens_(tokens), current_token_index_(0) {
    // initialize grammar rules...
}

ast::ASTNode::NodePtr CHTLParser::parse() {
    // Simplified entry point
    auto root = std::make_shared<ast::ASTNode>(ast::NodeType::ROOT);
    while (!is_at_end()) {
        root->add_child(parse_element());
    }
    return root;
}

ast::ASTNode::NodePtr CHTLParser::parse_element() {
    if (match_token(lexer::TokenType::AT)) {
        // This is a component usage, e.g. @Element Box from space
        consume_token(lexer::TokenType::AT);
        if (match_token(lexer::TokenType::KEYWORD_ELEMENT)) {
            consume_token(lexer::TokenType::KEYWORD_ELEMENT);
            auto element_name = consume_token(lexer::TokenType::IDENTIFIER);
            
            auto element_node = std::make_shared<ast::ElementNode>(element_name.value);
            
            if (match_token(lexer::TokenType::KEYWORD_FROM)) {
                consume_token(lexer::TokenType::KEYWORD_FROM);
                auto namespace_name = consume_token(lexer::TokenType::IDENTIFIER);
                element_node->from_namespace = namespace_name.value;
                std::cout << "Parsed element " << element_node->name << " from namespace " << element_node->from_namespace << std::endl;
            }
            consume_token(lexer::TokenType::SEMICOLON);
            return element_node;
        }
        // ... handle other @ usages like @Style ...
    }
    
    // This is a standard element definition, e.g. div { ... }
    if (match_token(lexer::TokenType::IDENTIFIER)) {
        auto tag_name = consume_token(lexer::TokenType::IDENTIFIER);
        auto element_node = std::make_shared<ast::ElementNode>(tag_name.value);

        if (match_token(lexer::TokenType::LEFT_BRACE)) {
            consume_token(lexer::TokenType::LEFT_BRACE);
            while (!match_token(lexer::TokenType::RIGHT_BRACE) && !is_at_end()) {
                 // Recursively parse child elements, attributes, etc.
                 // This part is complex and omitted for brevity.
                 // For now, let's just assume we can parse children.
                 // In a real implementation, you'd call parse_element(), parse_attributes() etc. here.
                 if (match_token(lexer::TokenType::IDENTIFIER)) {
                     // This is a simplification
                     element_node->add_child(parse_element());
                 } else {
                     next_token(); // Skip token to avoid infinite loop
                 }
            }
            consume_token(lexer::TokenType::RIGHT_BRACE);
        } else if (match_token(lexer::TokenType::SEMICOLON)) {
            consume_token(lexer::TokenType::SEMICOLON);
        }
        return element_node;
    }
    
    return nullptr;
}


ast::ASTNode::NodePtr CHTLParser::parse_import() {
    consume_token(lexer::TokenType::KEYWORD_IMPORT);

    // Check for granular import syntax like [Custom] @Element
    if (match_token(lexer::TokenType::LEFT_BRACKET)) {
        return parse_granular_import();
    }
    
    // Handle standard import like @Chtl from "path"
    if (match_token(lexer::TokenType::AT)) {
        consume_token(lexer::TokenType::AT);
        auto type_token = consume_token(lexer::TokenType::IDENTIFIER); // Chtl, Html, etc.
        consume_token(lexer::TokenType::KEYWORD_FROM);
        auto path_token = consume_token(lexer::TokenType::STRING_LITERAL); // Or unquoted literal
        
        ast::ImportNode::ImportType import_type = ast::ImportNode::ImportType::CHTL; // Default
        // ... logic to map type_token.value to enum ...
        
        auto import_node = std::make_shared<ast::ImportNode>(import_type, path_token.value);
        
        if (match_token(lexer::TokenType::KEYWORD_AS)) {
            consume_token(lexer::TokenType::KEYWORD_AS);
            import_node->alias = consume_token(lexer::TokenType::IDENTIFIER).value;
        }
        consume_token(lexer::TokenType::SEMICOLON);
        return import_node;
    }
    return nullptr;
}

ast::ASTNode::NodePtr CHTLParser::parse_granular_import() {
    consume_token(lexer::TokenType::LEFT_BRACKET);
    auto category_token = consume_token(lexer::TokenType::IDENTIFIER); // Custom, Template, etc.
    consume_token(lexer::TokenType::RIGHT_BRACKET);

    consume_token(lexer::TokenType::AT);
    auto type_token = consume_token(lexer::TokenType::IDENTIFIER); // Element, Style, etc.

    auto name_token = consume_token(lexer::TokenType::IDENTIFIER);

    consume_token(lexer::TokenType::KEYWORD_FROM);
    auto path_token = consume_token(lexer::TokenType::STRING_LITERAL);

    auto import_node = std::make_shared<ast::ImportNode>(ast::ImportNode::ImportType::CHTL, path_token.value);
    import_node->import_category = category_token.value;
    import_node->import_specifier = "@" + type_token.value;
    import_node->imported_item_name = name_token.value;

    if (match_token(lexer::TokenType::KEYWORD_AS)) {
        consume_token(lexer::TokenType::KEYWORD_AS);
        import_node->alias = consume_token(lexer::TokenType::IDENTIFIER).value;
    }
    consume_token(lexer::TokenType::SEMICOLON);

    std::cout << "Parsed granular import for " << import_node->imported_item_name << std::endl;
    return import_node;
}


// Dummy implementations for other methods to allow compilation
bool CHTLParser::is_at_end() const { return current_token_index_ >= tokens_.get_tokens().size(); }
const lexer::Token& CHTLParser::consume_token(lexer::TokenType type) { return tokens_.get_tokens()[current_token_index_++]; }
const lexer::Token& CHTLParser::next_token() { return tokens_.get_tokens()[current_token_index_++]; }
bool CHTLParser::match_token(lexer::TokenType type) const {
    if (is_at_end()) return false;
    return tokens_.get_tokens()[current_token_index_].type == type;
}

} // namespace parser
} // namespace chtl