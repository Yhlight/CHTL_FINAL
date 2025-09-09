#include "CHTLParser.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens), current_token(0) {
}

std::shared_ptr<RootNode> CHTLParser::parse() {
    auto root = std::make_shared<RootNode>();
    
    try {
        while (!isAtEnd()) {
            auto node = parseNode();
            if (node) {
                root->addRootContent(node);
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return nullptr;
    }
    
    return root;
}

bool CHTLParser::isAtEnd() const {
    return current_token >= tokens.size() || 
           tokens[current_token].token_type == TokenType::EndOfFile;
}

Token CHTLParser::current() const {
    if (isAtEnd()) {
        return Token(TokenType::EndOfFile, "", 0, 0, 0);
    }
    return tokens[current_token];
}

Token CHTLParser::peek() const {
    if (current_token + 1 >= tokens.size()) {
        return Token(TokenType::EndOfFile, "", 0, 0, 0);
    }
    return tokens[current_token + 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current_token++;
    }
    return tokens[current_token - 1];
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return current().token_type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    error(message);
    return Token(TokenType::EndOfFile, "", 0, 0, 0);
}

std::shared_ptr<CHTLNode> CHTLParser::parseNode() {
    if (isAtEnd()) return nullptr;
    
    Token token = current();
    
    switch (token.token_type) {
        case TokenType::Text:
            return parseTextNode();
        case TokenType::Style:
            return parseStyleNode();
        case TokenType::Script:
            return parseScriptNode();
        case TokenType::LeftBracket:
            // Check what's inside the bracket to determine type
            if (peek().token_type == TokenType::Template) {
                return parseTemplateNode();
            } else if (peek().token_type == TokenType::Custom) {
                return parseCustomNode();
            } else if (peek().token_type == TokenType::Import) {
                return parseImportNode();
            } else if (peek().token_type == TokenType::Namespace) {
                return parseNamespaceNode();
            } else if (peek().token_type == TokenType::Configuration) {
                return parseConfigurationNode();
            } else if (peek().token_type == TokenType::Origin) {
                return parseOriginNode();
            } else {
                error("Unknown bracket content: " + peek().value);
                return nullptr;
            }
        case TokenType::Use:
            return parseUseNode();
        case TokenType::Identifier:
            return parseElementNode();
        default:
            error("Unexpected token: " + token.value);
            return nullptr;
    }
}

std::shared_ptr<TextNode> CHTLParser::parseTextNode() {
    consume(TokenType::Text, "Expected 'text' keyword");
    consume(TokenType::LeftBrace, "Expected '{' after 'text'");
    
    std::string text_content;
    if (check(TokenType::StringLiteral)) {
        text_content = parseStringLiteral();
    } else if (check(TokenType::Identifier)) {
        text_content = parseUnquotedString();
    } else {
        error("Expected string literal or unquoted string in text node");
    }
    
    consume(TokenType::RightBrace, "Expected '}' after text content");
    
    return std::make_shared<TextNode>(text_content, current().line, current().column);
}

std::shared_ptr<ElementNode> CHTLParser::parseElementNode() {
    std::string tag_name = parseIdentifier();
    auto element = std::make_shared<ElementNode>(tag_name, current().line, current().column);
    
    if (match(TokenType::LeftBrace)) {
        // Parse attributes
        parseAttributes(element.get());
        
        // Parse content
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            auto child = parseNode();
            if (child) {
                element->addContent(child);
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after element content");
    }
    
    return element;
}

std::shared_ptr<AttributeNode> CHTLParser::parseAttribute() {
    std::string attr_name = parseIdentifier();
    consume(TokenType::Colon, "Expected ':' after attribute name");
    
    std::string attr_value;
    bool is_quoted = false;
    
    if (check(TokenType::StringLiteral)) {
        attr_value = parseStringLiteral();
        is_quoted = true;
    } else if (check(TokenType::Identifier) || check(TokenType::NumberLiteral)) {
        attr_value = current().value;
        advance();
        is_quoted = false;
    } else {
        error("Expected attribute value");
    }
    
    if (check(TokenType::Semicolon)) {
        advance(); // Consume semicolon
    }
    
    return std::make_shared<AttributeNode>(attr_name, attr_value, is_quoted, current().line, current().column);
}

std::shared_ptr<StyleNode> CHTLParser::parseStyleNode() {
    consume(TokenType::Style, "Expected 'style' keyword");
    consume(TokenType::LeftBrace, "Expected '{' after 'style'");
    
    auto style = std::make_shared<StyleNode>(current().line, current().column);
    parseStyleRules(style.get());
    
    consume(TokenType::RightBrace, "Expected '}' after style content");
    
    return style;
}

std::shared_ptr<ScriptNode> CHTLParser::parseScriptNode() {
    consume(TokenType::Script, "Expected 'script' keyword");
    consume(TokenType::LeftBrace, "Expected '{' after 'script'");
    
    auto script = std::make_shared<ScriptNode>(current().line, current().column);
    parseScriptContent(script.get());
    
    consume(TokenType::RightBrace, "Expected '}' after script content");
    
    return script;
}

std::shared_ptr<TemplateNode> CHTLParser::parseTemplateNode() {
    consume(TokenType::LeftBracket, "Expected '[' for template");
    consume(TokenType::Template, "Expected 'Template' keyword");
    consume(TokenType::RightBracket, "Expected ']' after 'Template'");
    consume(TokenType::At, "Expected '@' after template bracket");
    
    std::string template_type = parseIdentifier();
    std::string template_name = parseIdentifier();
    auto template_node = std::make_shared<TemplateNode>(template_type, template_name, current().line, current().column);
    
    if (match(TokenType::LeftBrace)) {
        parseTemplateContent(template_node.get());
        consume(TokenType::RightBrace, "Expected '}' after template content");
    }
    
    // Store template for later use
    templates[template_name] = template_node;
    
    return template_node;
}

std::shared_ptr<CustomNode> CHTLParser::parseCustomNode() {
    consume(TokenType::LeftBracket, "Expected '[' for custom");
    consume(TokenType::Custom, "Expected 'Custom' keyword");
    consume(TokenType::RightBracket, "Expected ']' after 'Custom'");
    consume(TokenType::At, "Expected '@' after custom bracket");
    
    std::string custom_type = parseIdentifier();
    std::string custom_name = parseIdentifier();
    auto custom_node = std::make_shared<CustomNode>(custom_type, custom_name, current().line, current().column);
    
    if (match(TokenType::LeftBrace)) {
        parseCustomContent(custom_node.get());
        consume(TokenType::RightBrace, "Expected '}' after custom content");
    }
    
    // Store custom for later use
    customs[custom_name] = custom_node;
    
    return custom_node;
}

std::shared_ptr<ImportNode> CHTLParser::parseImportNode() {
    consume(TokenType::Import, "Expected 'Import' keyword");
    consume(TokenType::LeftBracket, "Expected '[' after 'Import'");
    
    std::string import_type = parseIdentifier();
    consume(TokenType::RightBracket, "Expected ']' after import type");
    consume(TokenType::At, "Expected '@' after import type");
    
    std::string import_path = parseStringLiteral();
    std::string import_name = "";
    bool is_wildcard = false;
    
    if (match(TokenType::As)) {
        if (check(TokenType::Identifier)) {
            import_name = parseIdentifier();
        } else if (check(TokenType::Asterisk)) {
            advance(); // Consume '*'
            is_wildcard = true;
        }
    }
    
    return std::make_shared<ImportNode>(import_path, import_type, import_name, is_wildcard, current().line, current().column);
}

std::shared_ptr<NamespaceNode> CHTLParser::parseNamespaceNode() {
    consume(TokenType::Namespace, "Expected 'Namespace' keyword");
    consume(TokenType::LeftBracket, "Expected '[' after 'Namespace'");
    
    std::string namespace_name = parseIdentifier();
    consume(TokenType::RightBracket, "Expected ']' after namespace name");
    
    auto namespace_node = std::make_shared<NamespaceNode>(namespace_name, current().line, current().column);
    
    if (match(TokenType::LeftBrace)) {
        parseNamespaceContent(namespace_node.get());
        consume(TokenType::RightBrace, "Expected '}' after namespace content");
    }
    
    return namespace_node;
}

std::shared_ptr<ConfigurationNode> CHTLParser::parseConfigurationNode() {
    consume(TokenType::Configuration, "Expected 'Configuration' keyword");
    consume(TokenType::LeftBracket, "Expected '[' after 'Configuration'");
    
    std::string config_name = parseIdentifier();
    consume(TokenType::RightBracket, "Expected ']' after configuration name");
    
    auto config_node = std::make_shared<ConfigurationNode>(config_name, current().line, current().column);
    
    if (match(TokenType::LeftBrace)) {
        parseConfigurationOptions(config_node.get());
        consume(TokenType::RightBrace, "Expected '}' after configuration options");
    }
    
    // Store configuration for later use
    configurations[config_name] = config_node;
    
    return config_node;
}

std::shared_ptr<UseNode> CHTLParser::parseUseNode() {
    consume(TokenType::Use, "Expected 'use' keyword");
    
    std::string use_target = parseIdentifier();
    auto use_node = std::make_shared<UseNode>(use_target, current().line, current().column);
    
    if (match(TokenType::LeftBrace)) {
        parseUseContent(use_node.get());
        consume(TokenType::RightBrace, "Expected '}' after use content");
    }
    
    return use_node;
}

std::shared_ptr<OriginNode> CHTLParser::parseOriginNode() {
    consume(TokenType::Origin, "Expected 'Origin' keyword");
    consume(TokenType::LeftBracket, "Expected '[' after 'Origin'");
    
    std::string origin_type = parseIdentifier();
    consume(TokenType::RightBracket, "Expected ']' after origin type");
    consume(TokenType::At, "Expected '@' after origin type");
    
    std::string origin_name = parseIdentifier();
    consume(TokenType::LeftBrace, "Expected '{' after origin name");
    
    std::string origin_content;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        origin_content += current().value + " ";
        advance();
    }
    
    consume(TokenType::RightBrace, "Expected '}' after origin content");
    
    return std::make_shared<OriginNode>(origin_type, origin_name, origin_content, current().line, current().column);
}

std::string CHTLParser::parseStringLiteral() {
    if (check(TokenType::StringLiteral)) {
        std::string value = current().value;
        advance();
        return value;
    }
    error("Expected string literal");
    return "";
}

std::string CHTLParser::parseIdentifier() {
    if (check(TokenType::Identifier)) {
        std::string value = current().value;
        advance();
        return value;
    }
    error("Expected identifier");
    return "";
}

std::string CHTLParser::parseUnquotedString() {
    if (check(TokenType::Identifier)) {
        std::string value = current().value;
        advance();
        return value;
    }
    error("Expected unquoted string");
    return "";
}

void CHTLParser::parseAttributes(ElementNode* element) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (check(TokenType::Identifier)) {
            auto attr = parseAttribute();
            element->addAttribute(attr);
        } else if (check(TokenType::At)) {
            // Handle @Element or @Style references
            advance(); // consume @
            std::string ref_type = parseIdentifier();
            std::string ref_name = parseIdentifier();
            
            if (check(TokenType::Semicolon)) {
                advance(); // consume semicolon
            }
            
            // Create a special attribute for the reference
            auto ref_attr = std::make_shared<AttributeNode>("@" + ref_type, ref_name, false);
            element->addAttribute(ref_attr);
        } else {
            break;
        }
    }
}

void CHTLParser::parseStyleRules(StyleNode* style) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (check(TokenType::Dot) || check(TokenType::Hash) || check(TokenType::Identifier)) {
            // Parse CSS selector and rules
            // This is a simplified implementation
            std::string selector = current().value;
            advance();
            
            if (match(TokenType::LeftBrace)) {
                std::string rules;
                while (!check(TokenType::RightBrace) && !isAtEnd()) {
                    rules += current().value + " ";
                    advance();
                }
                consume(TokenType::RightBrace, "Expected '}' after CSS rules");
                
                // Create a simple text node for the CSS rules
                auto rule_node = std::make_shared<TextNode>(selector + " { " + rules + " }");
                style->addStyleRule(rule_node);
            }
        } else {
            advance(); // Skip unknown tokens
        }
    }
}

void CHTLParser::parseScriptContent(ScriptNode* script) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        std::string content = current().value;
        advance();
        auto content_node = std::make_shared<TextNode>(content);
        script->addScriptContent(content_node);
    }
}

void CHTLParser::parseTemplateContent(TemplateNode* template_node) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        // For now, treat template content as simple CSS-like rules
        std::string property = parseIdentifier();
        consume(TokenType::Colon, "Expected ':' after property");
        
        std::string value;
        if (check(TokenType::StringLiteral)) {
            value = parseStringLiteral();
        } else if (check(TokenType::NumberLiteral)) {
            value = current().value;
            advance();
            // Check for units like px
            if (check(TokenType::Identifier)) {
                value += current().value;
                advance();
            }
        } else if (check(TokenType::Identifier)) {
            value = parseIdentifier();
        }
        
        consume(TokenType::Semicolon, "Expected ';' after value");
        
        // Create a simple text node for the CSS property
        auto rule_node = std::make_shared<TextNode>(property + ": " + value + ";");
        template_node->addTemplateContent(rule_node);
    }
}

void CHTLParser::parseCustomContent(CustomNode* custom_node) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        auto content = parseNode();
        if (content) {
            custom_node->addCustomContent(content);
        }
    }
}

void CHTLParser::parseNamespaceContent(NamespaceNode* namespace_node) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        auto content = parseNode();
        if (content) {
            namespace_node->addNamespaceContent(content);
        }
    }
}

void CHTLParser::parseConfigurationOptions(ConfigurationNode* config) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (check(TokenType::Identifier)) {
            std::string key = parseIdentifier();
            consume(TokenType::Colon, "Expected ':' after configuration key");
            std::string value = parseStringLiteral();
            config->setConfigOption(key, value);
            
            if (check(TokenType::Semicolon)) {
                advance(); // Consume semicolon
            }
        } else {
            break;
        }
    }
}

void CHTLParser::parseUseContent(UseNode* use_node) {
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        auto content = parseNode();
        if (content) {
            use_node->addUseContent(content);
        }
    }
}

void CHTLParser::error(const std::string& message) {
    Token token = current();
    std::ostringstream oss;
    oss << "Parse error at line " << token.line << ", column " << token.column 
        << ": " << message << " (token: '" << token.value << "')";
    throw std::runtime_error(oss.str());
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (current().token_type == TokenType::Semicolon) {
            advance();
            return;
        }
        
        switch (current().token_type) {
            case TokenType::Text:
            case TokenType::Style:
            case TokenType::Script:
            case TokenType::Template:
            case TokenType::Custom:
            case TokenType::Import:
            case TokenType::Namespace:
            case TokenType::Configuration:
            case TokenType::Use:
            case TokenType::Origin:
                return;
            default:
                advance();
                break;
        }
    }
}

} // namespace CHTL