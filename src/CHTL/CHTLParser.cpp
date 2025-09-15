#include "CHTL/CHTLParser.h"
#include "CHTL/ConcreteNodes.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser() 
    : token_consumed_(true), debug_mode_(false) {
}

CHTLParser::~CHTLParser() = default;

void CHTLParser::setInput(const std::string& input) {
    lexer_.setInput(input);
    token_consumed_ = true;
    clearErrors();
}

void CHTLParser::setInput(std::istream& input) {
    lexer_.setInput(input);
    token_consumed_ = true;
    clearErrors();
}

std::shared_ptr<CHTLNode> CHTLParser::parse() {
    if (!lexer_.hasMoreTokens()) {
        return nullptr;
    }
    
    advance();
    
    // 根据第一个token决定解析类型
    if (isElementToken(current_token_.type)) {
        return parseElement();
    } else if (isTemplateToken(current_token_.type)) {
        return parseTemplate();
    } else if (isCustomToken(current_token_.type)) {
        return parseCustom();
    } else if (isOriginToken(current_token_.type)) {
        return parseOrigin();
    } else if (isImportToken(current_token_.type)) {
        return parseImport();
    } else if (isNamespaceToken(current_token_.type)) {
        return parseNamespace();
    } else if (isConfigurationToken(current_token_.type)) {
        return parseConfiguration();
    } else {
        reportError("Unexpected token: " + current_token_.value);
        return nullptr;
    }
}

std::vector<std::shared_ptr<CHTLNode>> CHTLParser::parseAll() {
    std::vector<std::shared_ptr<CHTLNode>> nodes;
    
    while (lexer_.hasMoreTokens()) {
        auto node = parse();
        if (node) {
            nodes.push_back(node);
        }
    }
    
    return nodes;
}

void CHTLParser::setContext(std::shared_ptr<CHTLContext> context) {
    context_ = context;
}

void CHTLParser::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
    lexer_.setDebugMode(enabled);
}

bool CHTLParser::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> CHTLParser::getErrors() const {
    return errors_;
}

void CHTLParser::clearErrors() {
    errors_.clear();
}

void CHTLParser::advance() {
    if (token_consumed_) {
        current_token_ = lexer_.nextToken();
        token_consumed_ = false;
    }
}

void CHTLParser::consume() {
    token_consumed_ = true;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        consume();
        return true;
    }
    return false;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    if (check(types)) {
        consume();
        return true;
    }
    return false;
}

bool CHTLParser::check(TokenType type) const {
    return current_token_.type == type;
}

bool CHTLParser::check(const std::vector<TokenType>& types) const {
    return std::find(types.begin(), types.end(), current_token_.type) != types.end();
}

void CHTLParser::reportError(const std::string& message) {
    std::string error = "Error at line " + std::to_string(current_token_.line) + 
                       ", column " + std::to_string(current_token_.column) + 
                       ": " + message;
    errors_.push_back(error);
    
    if (debug_mode_) {
        std::cerr << "[CHTLParser] " << error << std::endl;
    }
}

void CHTLParser::reportError(const std::string& message, const Token& token) {
    std::string error = "Error at line " + std::to_string(token.line) + 
                       ", column " + std::to_string(token.column) + 
                       ": " + message;
    errors_.push_back(error);
    
    if (debug_mode_) {
        std::cerr << "[CHTLParser] " << error << std::endl;
    }
}

std::shared_ptr<CHTLNode> CHTLParser::parseElement() {
    if (!isElementToken(current_token_.type)) {
        reportError("Expected element token");
        return nullptr;
    }
    
    std::string tag_name = current_token_.value;
    advance();
    
    auto element = std::make_shared<ElementNode>(tag_name);
    
    // 解析属性
    if (match(TokenType::LEFT_BRACE)) {
        parseAttributes(element);
        
        // 解析子节点
        while (!match(TokenType::RIGHT_BRACE)) {
            if (lexer_.hasMoreTokens()) {
                auto child = parse();
                if (child) {
                    element->addChild(child);
                }
            } else {
                reportError("Expected '}' to close element");
                break;
            }
        }
    }
    
    return element;
}

std::shared_ptr<CHTLNode> CHTLParser::parseText() {
    if (!match(TokenType::KEYWORD_TEXT)) {
        reportError("Expected 'text' keyword");
        return nullptr;
    }
    
    std::string content;
    
    if (match(TokenType::LEFT_BRACE)) {
        // 解析文本内容
        while (!match(TokenType::RIGHT_BRACE)) {
            if (current_token_.type == TokenType::STRING_LITERAL) {
                content += current_token_.value;
                advance();
            } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
                content += current_token_.value;
                advance();
            } else if (current_token_.type == TokenType::IDENTIFIER) {
                content += current_token_.value;
                advance();
            } else {
                advance();
            }
        }
    }
    
    return std::make_shared<TextNode>(content);
}

std::shared_ptr<CHTLNode> CHTLParser::parseComment() {
    // 注释解析
    if (current_token_.type == TokenType::SINGLE_LINE_COMMENT) {
        advance();
        return std::make_shared<CommentNode>("", CommentNode::CommentType::SINGLE_LINE);
    } else if (current_token_.type == TokenType::MULTI_LINE_COMMENT) {
        advance();
        return std::make_shared<CommentNode>("", CommentNode::CommentType::MULTI_LINE);
    } else if (current_token_.type == TokenType::GENERATOR_COMMENT) {
        advance();
        return std::make_shared<CommentNode>("", CommentNode::CommentType::GENERATOR);
    }
    
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyle() {
    if (!match(TokenType::KEYWORD_STYLE)) {
        reportError("Expected 'style' keyword");
        return nullptr;
    }
    
    auto style = std::make_shared<StyleNode>(StyleNode::StyleType::LOCAL);
    
    if (match(TokenType::LEFT_BRACE)) {
        parseCSSProperties(style);
        
        while (!match(TokenType::RIGHT_BRACE)) {
            if (lexer_.hasMoreTokens()) {
                advance();
            } else {
                reportError("Expected '}' to close style block");
                break;
            }
        }
    }
    
    return style;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScript() {
    if (!match(TokenType::KEYWORD_SCRIPT)) {
        reportError("Expected 'script' keyword");
        return nullptr;
    }
    
    auto script = std::make_shared<ScriptNode>(ScriptNode::ScriptType::LOCAL);
    
    if (match(TokenType::LEFT_BRACE)) {
        std::string content;
        
        while (!match(TokenType::RIGHT_BRACE)) {
            if (current_token_.type == TokenType::STRING_LITERAL) {
                content += current_token_.value;
                advance();
            } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
                content += current_token_.value;
                advance();
            } else if (current_token_.type == TokenType::IDENTIFIER) {
                content += current_token_.value;
                advance();
            } else {
                advance();
            }
        }
        
        script->setContent(content);
    }
    
    return script;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplate() {
    if (!match(TokenType::KEYWORD_TEMPLATE)) {
        reportError("Expected '[Template]' keyword");
        return nullptr;
    }
    
    // 解析模板类型
    if (match(TokenType::AT)) {
        if (current_token_.type == TokenType::KEYWORD_STYLE) {
            advance();
            return parseStyleTemplate();
        } else if (current_token_.type == TokenType::KEYWORD_AT) {
            advance();
            return parseElementTemplate();
        } else if (current_token_.type == TokenType::KEYWORD_AT) {
            advance();
            return parseVarTemplate();
        }
    }
    
    reportError("Expected template type (@Style, @Element, or @Var)");
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleTemplate() {
    if (current_token_.type != TokenType::IDENTIFIER) {
        reportError("Expected template name");
        return nullptr;
    }
    
    std::string templateName = current_token_.value;
    advance();
    
    auto templateNode = std::make_shared<TemplateNode>(templateName, TemplateNode::TemplateType::STYLE);
    
    if (match(TokenType::LEFT_BRACE)) {
        // 解析CSS属性
        while (!match(TokenType::RIGHT_BRACE)) {
            if (current_token_.type == TokenType::IDENTIFIER) {
                std::string property = current_token_.value;
                advance();
                
                if (match(TokenType::COLON)) {
                    std::string value;
                    
                    while (!check(TokenType::SEMICOLON) && !check(TokenType::RIGHT_BRACE) && lexer_.hasMoreTokens()) {
                        if (current_token_.type == TokenType::STRING_LITERAL) {
                            value += current_token_.value;
                            advance();
                        } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
                            value += current_token_.value;
                            advance();
                        } else if (current_token_.type == TokenType::IDENTIFIER) {
                            value += current_token_.value;
                            advance();
                        } else {
                            value += current_token_.value;
                            advance();
                        }
                    }
                    
                    templateNode->addCSSProperty(property, value);
                    
                    if (match(TokenType::SEMICOLON)) {
                        // 跳过分号
                    }
                }
            } else {
                advance();
            }
        }
    }
    
    return templateNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseElementTemplate() {
    if (current_token_.type != TokenType::IDENTIFIER) {
        reportError("Expected template name");
        return nullptr;
    }
    
    std::string templateName = current_token_.value;
    advance();
    
    auto templateNode = std::make_shared<TemplateNode>(templateName, TemplateNode::TemplateType::ELEMENT);
    
    if (match(TokenType::LEFT_BRACE)) {
        // 解析子元素
        while (!match(TokenType::RIGHT_BRACE)) {
            if (lexer_.hasMoreTokens()) {
                auto child = parse();
                if (child) {
                    templateNode->addChild(child);
                }
            } else {
                reportError("Expected '}' to close element template");
                break;
            }
        }
    }
    
    return templateNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseVarTemplate() {
    if (current_token_.type != TokenType::IDENTIFIER) {
        reportError("Expected template name");
        return nullptr;
    }
    
    std::string templateName = current_token_.value;
    advance();
    
    auto templateNode = std::make_shared<TemplateNode>(templateName, TemplateNode::TemplateType::VAR);
    
    if (match(TokenType::LEFT_BRACE)) {
        // 解析变量
        while (!match(TokenType::RIGHT_BRACE)) {
            if (current_token_.type == TokenType::IDENTIFIER) {
                std::string varName = current_token_.value;
                advance();
                
                if (match(TokenType::COLON) || match(TokenType::EQUALS)) {
                    std::string varValue;
                    
                    if (current_token_.type == TokenType::STRING_LITERAL) {
                        varValue = current_token_.value;
                        advance();
                    } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
                        varValue = current_token_.value;
                        advance();
                    } else if (current_token_.type == TokenType::IDENTIFIER) {
                        varValue = current_token_.value;
                        advance();
                    }
                    
                    templateNode->addVariable(varName, varValue);
                }
            } else {
                advance();
            }
        }
    }
    
    return templateNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustom() {
    if (!match(TokenType::KEYWORD_CUSTOM)) {
        reportError("Expected '[Custom]' keyword");
        return nullptr;
    }
    
    // 自定义解析逻辑
    // 这里应该实现完整的自定义解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOrigin() {
    if (!match(TokenType::KEYWORD_ORIGIN)) {
        reportError("Expected '[Origin]' keyword");
        return nullptr;
    }
    
    // 原始嵌入解析逻辑
    // 这里应该实现完整的原始嵌入解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImport() {
    if (!match(TokenType::KEYWORD_IMPORT)) {
        reportError("Expected '[Import]' keyword");
        return nullptr;
    }
    
    // 导入解析逻辑
    // 这里应该实现完整的导入解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespace() {
    if (!match(TokenType::KEYWORD_NAMESPACE)) {
        reportError("Expected '[Namespace]' keyword");
        return nullptr;
    }
    
    // 命名空间解析逻辑
    // 这里应该实现完整的命名空间解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfiguration() {
    if (!match(TokenType::KEYWORD_CONFIGURATION)) {
        reportError("Expected '[Configuration]' keyword");
        return nullptr;
    }
    
    // 配置解析逻辑
    // 这里应该实现完整的配置解析
    return nullptr;
}

void CHTLParser::parseAttributes(std::shared_ptr<ElementNode> element) {
    while (!check(TokenType::RIGHT_BRACE) && lexer_.hasMoreTokens()) {
        if (current_token_.type == TokenType::IDENTIFIER) {
            std::string attr_name = current_token_.value;
            advance();
            
            if (match(TokenType::COLON) || match(TokenType::EQUALS)) {
                std::string attr_value;
                
                if (current_token_.type == TokenType::STRING_LITERAL) {
                    attr_value = current_token_.value;
                    advance();
                } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
                    attr_value = current_token_.value;
                    advance();
                } else if (current_token_.type == TokenType::IDENTIFIER) {
                    attr_value = current_token_.value;
                    advance();
                }
                
                element->setAttribute(attr_name, attr_value);
            }
        } else {
            advance();
        }
    }
}

void CHTLParser::parseCSSProperties(std::shared_ptr<StyleNode> style) {
    while (!check(TokenType::RIGHT_BRACE) && lexer_.hasMoreTokens()) {
        if (current_token_.type == TokenType::IDENTIFIER) {
            std::string prop_name = current_token_.value;
            advance();
            
            if (match(TokenType::COLON)) {
                std::string prop_value;
                
                while (!check(TokenType::SEMICOLON) && !check(TokenType::RIGHT_BRACE) && lexer_.hasMoreTokens()) {
                    if (current_token_.type == TokenType::STRING_LITERAL) {
                        prop_value += current_token_.value;
                        advance();
                    } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
                        prop_value += current_token_.value;
                        advance();
                    } else if (current_token_.type == TokenType::IDENTIFIER) {
                        prop_value += current_token_.value;
                        advance();
                    } else {
                        prop_value += current_token_.value;
                        advance();
                    }
                }
                
                style->addCSSProperty(prop_name, prop_value);
                
                if (match(TokenType::SEMICOLON)) {
                    // 跳过分号
                }
            }
        } else {
            advance();
        }
    }
}

std::string CHTLParser::parseExpression() {
    return parseConditionalExpression();
}

std::string CHTLParser::parseConditionalExpression() {
    std::string left = parseLogicalOrExpression();
    
    if (match(TokenType::QUESTION)) {
        std::string true_expr = parseExpression();
        if (match(TokenType::COLON)) {
            std::string false_expr = parseExpression();
            return "(" + left + " ? " + true_expr + " : " + false_expr + ")";
        }
    }
    
    return left;
}

std::string CHTLParser::parseLogicalOrExpression() {
    std::string left = parseLogicalAndExpression();
    
    while (match(TokenType::OR)) {
        std::string right = parseLogicalAndExpression();
        left = "(" + left + " || " + right + ")";
    }
    
    return left;
}

std::string CHTLParser::parseLogicalAndExpression() {
    std::string left = parseEqualityExpression();
    
    while (match(TokenType::AND)) {
        std::string right = parseEqualityExpression();
        left = "(" + left + " && " + right + ")";
    }
    
    return left;
}

std::string CHTLParser::parseEqualityExpression() {
    std::string left = parseRelationalExpression();
    
    while (match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
        std::string op = current_token_.value;
        advance();
        std::string right = parseRelationalExpression();
        left = "(" + left + " " + op + " " + right + ")";
    }
    
    return left;
}

std::string CHTLParser::parseRelationalExpression() {
    std::string left = parseAdditiveExpression();
    
    while (match({TokenType::LESS_THAN, TokenType::GREATER_THAN, 
                  TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
        std::string op = current_token_.value;
        advance();
        std::string right = parseAdditiveExpression();
        left = "(" + left + " " + op + " " + right + ")";
    }
    
    return left;
}

std::string CHTLParser::parseAdditiveExpression() {
    std::string left = parseMultiplicativeExpression();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = current_token_.value;
        advance();
        std::string right = parseMultiplicativeExpression();
        left = "(" + left + " " + op + " " + right + ")";
    }
    
    return left;
}

std::string CHTLParser::parseMultiplicativeExpression() {
    std::string left = parseUnaryExpression();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO})) {
        std::string op = current_token_.value;
        advance();
        std::string right = parseUnaryExpression();
        left = "(" + left + " " + op + " " + right + ")";
    }
    
    return left;
}

std::string CHTLParser::parseUnaryExpression() {
    if (match({TokenType::PLUS, TokenType::MINUS, TokenType::NOT})) {
        std::string op = current_token_.value;
        advance();
        return op + parseUnaryExpression();
    }
    
    return parsePrimaryExpression();
}

std::string CHTLParser::parsePrimaryExpression() {
    if (current_token_.type == TokenType::IDENTIFIER) {
        std::string value = current_token_.value;
        advance();
        return value;
    } else if (current_token_.type == TokenType::STRING_LITERAL) {
        std::string value = current_token_.value;
        advance();
        return value;
    } else if (current_token_.type == TokenType::NUMBER_LITERAL) {
        std::string value = current_token_.value;
        advance();
        return value;
    } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
        std::string value = current_token_.value;
        advance();
        return value;
    } else if (match(TokenType::LEFT_PAREN)) {
        std::string expr = parseExpression();
        if (match(TokenType::RIGHT_PAREN)) {
            return "(" + expr + ")";
        }
    }
    
    return "";
}

std::string CHTLParser::getTokenValue(TokenType type) const {
    switch (type) {
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COLON: return ":";
        case TokenType::EQUALS: return "=";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        default: return "";
    }
}

bool CHTLParser::isElementToken(TokenType type) const {
    return type >= TokenType::KEYWORD_HTML && type <= TokenType::KEYWORD_TEXT;
}

bool CHTLParser::isTemplateToken(TokenType type) const {
    return type == TokenType::KEYWORD_TEMPLATE;
}

bool CHTLParser::isCustomToken(TokenType type) const {
    return type == TokenType::KEYWORD_CUSTOM;
}

bool CHTLParser::isOriginToken(TokenType type) const {
    return type == TokenType::KEYWORD_ORIGIN;
}

bool CHTLParser::isImportToken(TokenType type) const {
    return type == TokenType::KEYWORD_IMPORT;
}

bool CHTLParser::isNamespaceToken(TokenType type) const {
    return type == TokenType::KEYWORD_NAMESPACE;
}

bool CHTLParser::isConfigurationToken(TokenType type) const {
    return type == TokenType::KEYWORD_CONFIGURATION;
}

} // namespace CHTL