#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser() 
    : current_token_(0), debug_mode_(false) {
}

std::shared_ptr<CHTLNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    tokens_ = tokens;
    current_token_ = 0;
    errors_.clear();
    
    try {
        return parseRoot();
    } catch (const std::exception& e) {
        reportError(e.what());
        return nullptr;
    }
}

std::shared_ptr<CHTLNode> CHTLParser::parse(const std::string& source_code) {
    CHTLLexer lexer;
    auto tokens = lexer.tokenize(source_code);
    return parse(tokens);
}

void CHTLParser::reset() {
    tokens_.clear();
    current_token_ = 0;
    errors_.clear();
}

void CHTLParser::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}


void CHTLParser::clearErrors() {
    errors_.clear();
}

std::shared_ptr<CHTLNode> CHTLParser::parseRoot() {
    auto root = std::make_shared<ElementNode>("root");
    root->setName("root");
    
    while (!isAtEnd() && current().type != TokenType::EOF_TOKEN) {
        try {
            auto node = parseElement();
            if (node) {
                root->addChild(node);
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return root;
}

std::shared_ptr<CHTLNode> CHTLParser::parseElement() {
    if (isAtEnd()) return nullptr;
    
    Token current_token = current();
    
    // 检查模板声明
    if (current_token.type == TokenType::TEMPLATE) {
        return parseTemplate();
    }
    
    // 检查自定义声明
    if (current_token.type == TokenType::CUSTOM) {
        return parseCustom();
    }
    
    // 检查原始嵌入
    if (current_token.type == TokenType::ORIGIN) {
        return parseOrigin();
    }
    
    // 检查导入
    if (current_token.type == TokenType::IMPORT) {
        return parseImport();
    }
    
    // 检查配置
    if (current_token.type == TokenType::CONFIGURATION) {
        return parseConfiguration();
    }
    
    // 检查命名空间
    if (current_token.type == TokenType::NAMESPACE) {
        return parseNamespace();
    }
    
    // 检查use语句
    if (current_token.type == TokenType::USE) {
        consume(); // 消费use
        if (check(TokenType::IDENTIFIER)) {
            std::string config_name = consume().value;
            // 处理use语句
            return nullptr; // 暂时返回nullptr
        }
    }
    
    // 检查文本节点
    if (current_token.type == TokenType::TEXT) {
        return parseText();
    }
    
    // 普通元素
    if (current_token.type == TokenType::IDENTIFIER) {
        std::string tag_name = consume().value;
        auto element = std::make_shared<ElementNode>(tag_name);
        element->setPosition(current_token.line, current_token.column);
        
        // 解析属性
        parseAttributes(element);
        
        // 解析子节点
        if (check(TokenType::LBRACE)) {
            consume(); // 消费 {
            
            while (!isAtEnd() && !check(TokenType::RBRACE)) {
                auto child = parseElement();
                if (child) {
                    element->addChild(child);
                }
            }
            
            if (check(TokenType::RBRACE)) {
                consume(); // 消费 }
            } else {
                reportError("Expected '}' after element body");
            }
        }
        
        return element;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseText() {
    if (!check(TokenType::TEXT)) return nullptr;
    
    consume(); // 消费text
    
    auto text_node = std::make_shared<TextNode>("");
    text_node->setPosition(current().line, current().column);
    
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        std::string content;
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            if (current().type == TokenType::STRING) {
                content += current().value;
            } else if (current().type == TokenType::IDENTIFIER) {
                content += current().value;
            }
            consume();
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        }
        
        text_node->setTextContent(content);
    }
    
    return text_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseComment() {
    // 注释在词法分析阶段已经被处理
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplate() {
    if (!check(TokenType::TEMPLATE)) return nullptr;
    
    consume(); // 消费 [Template]
    
    // 解析模板类型
    TokenType template_type = current().type;
    if (template_type != TokenType::STYLE_TEMPLATE && 
        template_type != TokenType::ELEMENT_TEMPLATE && 
        template_type != TokenType::VAR_TEMPLATE) {
        reportError("Expected template type (@Style, @Element, or @Var)");
        return nullptr;
    }
    
    std::string type_name = consume().value;
    
    // 解析模板名称
    if (!check(TokenType::IDENTIFIER)) {
        reportError("Expected template name");
        return nullptr;
    }
    
    std::string template_name = consume().value;
    
    auto template_node = std::make_shared<TemplateNode>(template_name, type_name);
    template_node->setPosition(current().line, current().column);
    
    // 解析模板体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                template_node->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after template body");
        }
    }
    
    return template_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustom() {
    if (!check(TokenType::CUSTOM)) return nullptr;
    
    consume(); // 消费 [Custom]
    
    // 解析自定义类型
    TokenType custom_type = current().type;
    if (custom_type != TokenType::STYLE_TEMPLATE && 
        custom_type != TokenType::ELEMENT_TEMPLATE && 
        custom_type != TokenType::VAR_TEMPLATE) {
        reportError("Expected custom type (@Style, @Element, or @Var)");
        return nullptr;
    }
    
    std::string type_name = consume().value;
    
    // 解析自定义名称
    if (!check(TokenType::IDENTIFIER)) {
        reportError("Expected custom name");
        return nullptr;
    }
    
    std::string custom_name = consume().value;
    
    auto custom_node = std::make_shared<CustomNode>(custom_name, type_name);
    custom_node->setPosition(current().line, current().column);
    
    // 解析自定义体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                custom_node->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after custom body");
        }
    }
    
    return custom_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOrigin() {
    if (!check(TokenType::ORIGIN)) return nullptr;
    
    consume(); // 消费 [Origin]
    
    // 解析原始嵌入类型
    TokenType origin_type = current().type;
    if (origin_type != TokenType::HTML_ORIGIN && 
        origin_type != TokenType::STYLE_ORIGIN && 
        origin_type != TokenType::JS_ORIGIN) {
        reportError("Expected origin type (@Html, @Style, or @JavaScript)");
        return nullptr;
    }
    
    std::string type_name = consume().value;
    
    auto origin_node = std::make_shared<OriginNode>(type_name);
    origin_node->setPosition(current().line, current().column);
    
    // 解析原始嵌入体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        std::string content;
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            content += current().value + " ";
            consume();
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after origin body");
        }
        
        origin_node->setTextContent(content);
    }
    
    return origin_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImport() {
    if (!check(TokenType::IMPORT)) return nullptr;
    
    consume(); // 消费 [Import]
    
    // 解析导入类型
    TokenType import_type = current().type;
    if (import_type != TokenType::HTML_IMPORT && 
        import_type != TokenType::STYLE_IMPORT && 
        import_type != TokenType::JS_IMPORT &&
        import_type != TokenType::CHTL_IMPORT &&
        import_type != TokenType::CJMOD_IMPORT) {
        reportError("Expected import type");
        return nullptr;
    }
    
    std::string type_name = consume().value;
    
    // 解析from关键字
    if (!check(TokenType::FROM)) {
        reportError("Expected 'from' keyword");
        return nullptr;
    }
    consume(); // 消费from
    
    // 解析路径
    std::string path = parsePath();
    
    auto import_node = std::make_shared<ImportNode>(type_name, path);
    import_node->setPosition(current().line, current().column);
    
    return import_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfiguration() {
    if (!check(TokenType::CONFIGURATION)) return nullptr;
    
    consume(); // 消费 [Configuration]
    
    auto config_node = std::make_shared<ElementNode>("configuration");
    config_node->setPosition(current().line, current().column);
    
    // 解析配置体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                config_node->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after configuration body");
        }
    }
    
    return config_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespace() {
    if (!check(TokenType::NAMESPACE)) return nullptr;
    
    consume(); // 消费 [Namespace]
    
    // 解析命名空间名称
    if (!check(TokenType::IDENTIFIER)) {
        reportError("Expected namespace name");
        return nullptr;
    }
    
    std::string namespace_name = consume().value;
    
    auto namespace_node = std::make_shared<ElementNode>("namespace");
    namespace_node->setName(namespace_name);
    namespace_node->setPosition(current().line, current().column);
    
    // 解析命名空间体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                namespace_node->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after namespace body");
        }
    }
    
    return namespace_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyle() {
    if (!check(TokenType::STYLE)) return nullptr;
    
    consume(); // 消费style
    
    auto style_node = std::make_shared<StyleNode>();
    style_node->setPosition(current().line, current().column);
    
    // 解析样式体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                style_node->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after style body");
        }
    }
    
    return style_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScript() {
    if (!check(TokenType::SCRIPT)) return nullptr;
    
    consume(); // 消费script
    
    auto script_node = std::make_shared<ScriptNode>();
    script_node->setPosition(current().line, current().column);
    
    // 解析脚本体
    if (check(TokenType::LBRACE)) {
        consume(); // 消费 {
        
        std::string content;
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            content += current().value + " ";
            consume();
        }
        
        if (check(TokenType::RBRACE)) {
            consume(); // 消费 }
        } else {
            reportError("Expected '}' after script body");
        }
        
        script_node->setTextContent(content);
    }
    
    return script_node;
}

void CHTLParser::parseAttributes(std::shared_ptr<CHTLNode> node) {
    while (!isAtEnd() && current().type == TokenType::IDENTIFIER) {
        std::string attr_name = consume().value;
        
        if (check(TokenType::COLON) || check(TokenType::EQUAL)) {
            consume(); // 消费 : 或 =
            
            AttributeValue attr_value = parseAttributeValue();
            node->setAttribute(attr_name, attr_value);
            
            if (check(TokenType::SEMICOLON)) {
                consume(); // 消费 ;
            }
        }
    }
}

AttributeValue CHTLParser::parseAttributeValue() {
    Token current_token = current();
    
    if (current_token.type == TokenType::STRING) {
        consume();
        return current_token.value;
    } else if (current_token.type == TokenType::NUMBER) {
        consume();
        return std::stod(current_token.value);
    } else if (current_token.type == TokenType::IDENTIFIER) {
        consume();
        return current_token.value;
    }
    
    return std::string("");
}

std::string CHTLParser::parsePath() {
    if (current().type == TokenType::STRING) {
        std::string path = current().value;
        consume();
        return path;
    } else if (current().type == TokenType::IDENTIFIER) {
        std::string path = current().value;
        consume();
        return path;
    }
    
    return "";
}

std::string CHTLParser::parseIdentifier() {
    if (check(TokenType::IDENTIFIER)) {
        std::string identifier = current().value;
        consume();
        return identifier;
    }
    
    return "";
}

std::string CHTLParser::parseString() {
    if (check(TokenType::STRING)) {
        std::string str = current().value;
        consume();
        return str;
    }
    
    return "";
}

double CHTLParser::parseNumber() {
    if (check(TokenType::NUMBER)) {
        double num = std::stod(current().value);
        consume();
        return num;
    }
    
    return 0.0;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return current().type == type;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        consume();
        return true;
    }
    return false;
}

Token CHTLParser::consume() {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    
    return tokens_[current_token_++];
}

Token CHTLParser::consume(TokenType type) {
    if (check(type)) {
        return consume();
    }
    
    error("Expected " + std::to_string(static_cast<int>(type)) + " but got " + std::to_string(static_cast<int>(current().type)));
    return Token();
}

const Token& CHTLParser::current() const {
    if (isAtEnd()) {
        static Token eof_token(TokenType::EOF_TOKEN, "", 0, 0, 0);
        return eof_token;
    }
    return tokens_[current_token_];
}

const Token& CHTLParser::previous() const {
    if (current_token_ == 0) {
        static Token empty_token(TokenType::ERROR, "", 0, 0, 0);
        return empty_token;
    }
    return tokens_[current_token_ - 1];
}

bool CHTLParser::isAtEnd() const {
    return current_token_ >= tokens_.size() || current().type == TokenType::EOF_TOKEN;
}

void CHTLParser::synchronize() {
    consume();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (current().type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
            case TokenType::ORIGIN:
            case TokenType::IMPORT:
            case TokenType::CONFIGURATION:
            case TokenType::NAMESPACE:
                return;
            default:
                break;
        }
        
        consume();
    }
}

void CHTLParser::reportError(const std::string& message) {
    errors_.push_back("Error at line " + std::to_string(current().line) + 
                     ", column " + std::to_string(current().column) + ": " + message);
}

void CHTLParser::reportError(const Token& token, const std::string& message) {
    errors_.push_back("Error at line " + std::to_string(token.line) + 
                     ", column " + std::to_string(token.column) + ": " + message);
}

void CHTLParser::error(const std::string& message) {
    reportError(message);
    throw std::runtime_error(message);
}

void CHTLParser::error(const Token& token, const std::string& message) {
    reportError(token, message);
    throw std::runtime_error(message);
}

} // namespace CHTL