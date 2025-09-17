#include "CHTLParser.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<CHTL_Token>& tokens)
    : tokens_(tokens), currentTokenIndex_(0) {
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parse() {
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        throw std::runtime_error("Parse error: " + std::string(e.what()));
    }
}

CHTL_Token CHTLParser::peek(int offset) const {
    int index = currentTokenIndex_ + offset;
    if (index >= 0 && index < tokens_.size()) {
        return tokens_[index];
    }
    return CHTL_Token(CHTL_TokenType::END_OF_FILE, "", 0, 0);
}

CHTL_Token CHTLParser::consume() {
    if (currentTokenIndex_ < tokens_.size()) {
        return tokens_[currentTokenIndex_++];
    }
    return CHTL_Token(CHTL_TokenType::END_OF_FILE, "", 0, 0);
}

void CHTLParser::expect(CHTL_TokenType type, const std::string& errorMessage) {
    CHTL_Token token = peek();
    if (token.type != type) {
        throw std::runtime_error(errorMessage + " (expected " + 
                               std::to_string(static_cast<int>(type)) + 
                               ", got " + std::to_string(static_cast<int>(token.type)) + ")");
    }
    consume();
}

void CHTLParser::expect(const std::vector<CHTL_TokenType>& types, const std::string& errorMessage) {
    CHTL_Token token = peek();
    if (std::find(types.begin(), types.end(), token.type) == types.end()) {
        throw std::runtime_error(errorMessage);
    }
    consume();
}

bool CHTLParser::match(CHTL_TokenType type) {
    return peek().type == type;
}

bool CHTLParser::match(const std::vector<CHTL_TokenType>& types) {
    CHTL_Token token = peek();
    return std::find(types.begin(), types.end(), token.type) != types.end();
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseDocument() {
    auto document = std::make_unique<CHTLBaseNode>(CHTL_NodeType::DOCUMENT);
    
    while (currentTokenIndex_ < tokens_.size() && peek().type != CHTL_TokenType::END_OF_FILE) {
        auto statement = parseStatement();
        if (statement) {
            document->addChild(statement);
        }
    }
    
    return document;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseStatement() {
    CHTL_Token token = peek();
    
    switch (token.type) {
        case CHTL_TokenType::KEYWORD_TEXT:
            return parseTextNode();
        case CHTL_TokenType::KEYWORD_STYLE:
            return parseStyleBlock(false);
        case CHTL_TokenType::KEYWORD_SCRIPT:
            return parseScriptBlock(false);
        case CHTL_TokenType::KEYWORD_TEMPLATE:
            return parseTemplateDefinition();
        case CHTL_TokenType::KEYWORD_CUSTOM:
            return parseCustomDefinition();
        case CHTL_TokenType::KEYWORD_NAMESPACE:
            return parseNamespaceDefinition();
        case CHTL_TokenType::KEYWORD_CONFIG:
            return parseConfigDefinition();
        case CHTL_TokenType::KEYWORD_IMPORT:
            return parseImportStatement();
        case CHTL_TokenType::KEYWORD_USE:
            return parseUseStatement();
        case CHTL_TokenType::KEYWORD_DELETE:
            return parseDeleteOperator();
        case CHTL_TokenType::KEYWORD_INSERT:
            return parseInsertOperator();
        case CHTL_TokenType::CHTL_COMMENT_GENERATOR:
            return parseGeneratorComment();
        case CHTL_TokenType::IDENTIFIER:
            // 可能是HTML元素
            return parseElement();
        default:
            // 尝试解析表达式
            return parseExpression();
    }
}

std::unique_ptr<CHTLElementNode> CHTLParser::parseElement() {
    CHTL_Token token = consume();
    if (token.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected element name");
    }
    
    auto element = std::make_unique<CHTLElementNode>(token.value);
    
    // 解析属性
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            if (match(CHTL_TokenType::KEYWORD_TEXT)) {
                // 解析text属性
                auto textNode = parseTextNode();
                if (textNode) {
                    element->addChild(textNode);
                }
            } else if (match(CHTL_TokenType::KEYWORD_STYLE)) {
                // 解析局部样式块
                auto styleBlock = parseStyleBlock(false);
                if (styleBlock) {
                    element->addChild(styleBlock);
                }
            } else if (match(CHTL_TokenType::KEYWORD_SCRIPT)) {
                // 解析局部脚本块
                auto scriptBlock = parseScriptBlock(false);
                if (scriptBlock) {
                    element->addChild(scriptBlock);
                }
            } else if (match(CHTL_TokenType::IDENTIFIER)) {
                // 解析属性
                auto attribute = parseAttribute();
                if (attribute) {
                    element->addAttribute(attribute);
                }
            } else if (match(CHTL_TokenType::CHTL_TEMPLATE_VARIABLE) ||
                      match(CHTL_TokenType::CHTL_CUSTOM_VARIABLE) ||
                      match(CHTL_TokenType::CHTL_CUSTOM_VARIABLE_SPECIALIZATION)) {
                // 解析CHTL特殊语法
                auto specialSyntax = parseCHTLSpecialSyntax();
                if (specialSyntax) {
                    element->addChild(specialSyntax);
                }
            } else if (match(CHTL_TokenType::IDENTIFIER)) {
                // 可能是子元素
                auto childElement = parseElement();
                if (childElement) {
                    element->addChild(childElement);
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return element;
}

std::unique_ptr<CHTLAttributeNode> CHTLParser::parseAttribute() {
    CHTL_Token nameToken = consume();
    if (nameToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected attribute name");
    }
    
    // 解析冒号或等号
    if (match(CHTL_TokenType::OPERATOR_COLON) || match(CHTL_TokenType::OPERATOR_EQUAL)) {
        consume();
    }
    
    // 解析属性值
    auto value = parseAttributeValue();
    
    return std::make_unique<CHTLAttributeNode>(nameToken.value, value);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseAttributeValue() {
    CHTL_Token token = peek();
    
    switch (token.type) {
        case CHTL_TokenType::STRING_LITERAL:
        case CHTL_TokenType::UNQUOTED_LITERAL:
        case CHTL_TokenType::NUMBER_LITERAL:
            consume();
            return std::make_unique<CHTLBaseNode>(CHTL_NodeType::LITERAL);
        case CHTL_TokenType::CHTL_TEMPLATE_VARIABLE:
        case CHTL_TokenType::CHTL_CUSTOM_VARIABLE:
        case CHTL_TokenType::CHTL_CUSTOM_VARIABLE_SPECIALIZATION:
        case CHTL_TokenType::CHTL_TEMPLATE_STYLE_GROUP:
        case CHTL_TokenType::CHTL_CUSTOM_STYLE_GROUP:
        case CHTL_TokenType::CHTL_CUSTOM_STYLE_GROUP_SPECIALIZATION:
        case CHTL_TokenType::CHTL_NO_VALUE_STYLE_GROUP:
            return parseCHTLSpecialSyntax();
        default:
            // 解析表达式
            return parseExpression();
    }
}

std::unique_ptr<CHTLTextNode> CHTLParser::parseTextNode() {
    expect(CHTL_TokenType::KEYWORD_TEXT, "Expected 'text'");
    
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        // text { ... } 形式
        consume(); // 消费 {
        
        std::string content;
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token token = consume();
            if (token.type == CHTL_TokenType::STRING_LITERAL ||
                token.type == CHTL_TokenType::UNQUOTED_LITERAL) {
                content += token.value;
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
        return std::make_unique<CHTLTextNode>(content);
    } else {
        // text: "..." 形式
        expect(CHTL_TokenType::OPERATOR_COLON, "Expected ':'");
        
        CHTL_Token valueToken = consume();
        if (valueToken.type != CHTL_TokenType::STRING_LITERAL &&
            valueToken.type != CHTL_TokenType::UNQUOTED_LITERAL) {
            throw std::runtime_error("Expected string literal");
        }
        
        return std::make_unique<CHTLTextNode>(valueToken.value);
    }
}

std::unique_ptr<CHTLCommentNode> CHTLParser::parseComment() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLCommentNode>(token.value, token.type);
}

std::unique_ptr<CHTLStyleBlockNode> CHTLParser::parseStyleBlock(bool isGlobal) {
    expect(CHTL_TokenType::KEYWORD_STYLE, "Expected 'style'");
    
    auto styleBlock = std::make_unique<CHTLStyleBlockNode>(isGlobal);
    
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            auto styleGroup = parseStyleGroup();
            if (styleGroup) {
                styleBlock->addStyleGroup(styleGroup);
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return styleBlock;
}

std::unique_ptr<CHTLStyleGroupNode> CHTLParser::parseStyleGroup(const std::string& initialSelector) {
    std::string selector = initialSelector;
    
    // 解析选择器
    if (selector.empty()) {
        CHTL_Token token = peek();
        if (token.type == CHTL_TokenType::IDENTIFIER || 
            token.type == CHTL_TokenType::OPERATOR_DOT ||
            token.type == CHTL_TokenType::OPERATOR_COLON) {
            selector = token.value;
            consume();
        }
    }
    
    auto styleGroup = std::make_unique<CHTLStyleGroupNode>(selector);
    
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token token = peek();
            
            if (token.type == CHTL_TokenType::KEYTL_INHERIT) {
                // 解析继承
                auto inheritanceList = parseInheritanceList();
                for (const auto& inherited : inheritanceList) {
                    styleGroup->addInheritance(inherited);
                }
            } else if (token.type == CHTL_TokenType::KEYWORD_DELETE) {
                // 解析删除操作
                consume(); // 消费 delete
                // 解析要删除的属性或继承
                // 这里需要更复杂的解析逻辑
            } else if (token.type == CHTL_TokenType::IDENTIFIER) {
                // 解析样式属性
                auto property = parseStyleProperty();
                if (property) {
                    styleGroup->addProperty(property);
                }
            } else if (token.type == CHTL_TokenType::OPERATOR_DOT ||
                      token.type == CHTL_TokenType::OPERATOR_COLON) {
                // 解析嵌套样式组
                auto nestedGroup = parseStyleGroup();
                if (nestedGroup) {
                    styleGroup->addNestedGroup(nestedGroup);
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return styleGroup;
}

std::unique_ptr<CHTLStylePropertyNode> CHTLParser::parseStyleProperty() {
    CHTL_Token keyToken = consume();
    if (keyToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected property name");
    }
    
    expect(CHTL_TokenType::OPERATOR_COLON, "Expected ':'");
    
    // 解析属性值
    auto value = parseStyleValue();
    
    expect(CHTL_TokenType::OPERATOR_SEMICOLON, "Expected ';'");
    
    return std::make_unique<CHTLStylePropertyNode>(keyToken.value, value);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseStyleValue() {
    CHTL_Token token = peek();
    
    switch (token.type) {
        case CHTL_TokenType::STRING_LITERAL:
        case CHTL_TokenType::UNQUOTED_LITERAL:
        case CHTL_TokenType::NUMBER_LITERAL:
            consume();
            return std::make_unique<CHTLBaseNode>(CHTL_NodeType::LITERAL);
        case CHTL_TokenType::CHTL_TEMPLATE_VARIABLE:
        case CHTL_TokenType::CHTL_CUSTOM_VARIABLE:
        case CHTL_TokenType::CHTL_CUSTOM_VARIABLE_SPECIALIZATION:
        case CHTL_TokenType::CHTL_TEMPLATE_STYLE_GROUP:
        case CHTL_TokenType::CHTL_CUSTOM_STYLE_GROUP:
        case CHTL_TokenType::CHTL_CUSTOM_STYLE_GROUP_SPECIALIZATION:
        case CHTL_TokenType::CHTL_NO_VALUE_STYLE_GROUP:
            return parseCHTLSpecialSyntax();
        default:
            // 解析表达式（可能包含算术运算）
            return parseExpression();
    }
}

std::vector<std::string> CHTLParser::parseInheritanceList() {
    std::vector<std::string> inheritanceList;
    
    expect(CHTL_TokenType::KEYTL_INHERIT, "Expected 'inherit'");
    expect(CHTL_TokenType::PAREN_OPEN, "Expected '('");
    
    while (!match(CHTL_TokenType::PAREN_CLOSE) && currentTokenIndex_ < tokens_.size()) {
        CHTL_Token token = consume();
        if (token.type == CHTL_TokenType::IDENTIFIER) {
            inheritanceList.push_back(token.value);
        }
        
        if (match(CHTL_TokenType::OPERATOR_COMMA)) {
            consume(); // 消费逗号
        }
    }
    
    expect(CHTL_TokenType::PAREN_CLOSE, "Expected ')'");
    
    return inheritanceList;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseScriptBlock(bool isGlobal) {
    expect(CHTL_TokenType::KEYWORD_SCRIPT, "Expected 'script'");
    
    auto scriptBlock = std::make_unique<CHTLBaseNode>(CHTL_NodeType::SCRIPT_BLOCK);
    
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        std::string scriptContent;
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token token = consume();
            scriptContent += token.value + " ";
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
        
        // 设置脚本内容
        scriptBlock->setAttribute("content", scriptContent);
    }
    
    return scriptBlock;
}

std::unique_ptr<CHTLTemplateNode> CHTLParser::parseTemplateDefinition() {
    expect(CHTL_TokenType::KEYWORD_TEMPLATE, "Expected 'template'");
    
    CHTL_Token nameToken = consume();
    if (nameToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected template name");
    }
    
    auto templateNode = std::make_unique<CHTLTemplateNode>(nameToken.value);
    
    // 解析参数
    if (match(CHTL_TokenType::PAREN_OPEN)) {
        consume(); // 消费 (
        
        while (!match(CHTL_TokenType::PAREN_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token paramToken = consume();
            if (paramToken.type == CHTL_TokenType::IDENTIFIER) {
                templateNode->addParameter(paramToken.value);
            }
            
            if (match(CHTL_TokenType::OPERATOR_COMMA)) {
                consume(); // 消费逗号
            }
        }
        
        expect(CHTL_TokenType::PAREN_CLOSE, "Expected ')'");
    }
    
    // 解析模板体
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            auto bodyNode = parseStatement();
            if (bodyNode) {
                templateNode->addBodyNode(bodyNode);
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return templateNode;
}

std::unique_ptr<CHTLCustomNode> CHTLParser::parseCustomDefinition() {
    expect(CHTL_TokenType::KEYWORD_CUSTOM, "Expected 'custom'");
    
    CHTL_Token nameToken = consume();
    if (nameToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected custom name");
    }
    
    auto customNode = std::make_unique<CHTLCustomNode>(nameToken.value);
    
    // 解析参数
    if (match(CHTL_TokenType::PAREN_OPEN)) {
        consume(); // 消费 (
        
        while (!match(CHTL_TokenType::PAREN_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token paramToken = consume();
            if (paramToken.type == CHTL_TokenType::IDENTIFIER) {
                customNode->addParameter(paramToken.value);
            }
            
            if (match(CHTL_TokenType::OPERATOR_COMMA)) {
                consume(); // 消费逗号
            }
        }
        
        expect(CHTL_TokenType::PAREN_CLOSE, "Expected ')'");
    }
    
    // 解析自定义体
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            auto bodyNode = parseStatement();
            if (bodyNode) {
                customNode->addBodyNode(bodyNode);
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return customNode;
}

std::unique_ptr<CHTLUseNode> CHTLParser::parseUseStatement() {
    expect(CHTL_TokenType::KEYWORD_USE, "Expected 'use'");
    
    CHTL_Token nameToken = consume();
    if (nameToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected use target");
    }
    
    auto useNode = std::make_unique<CHTLUseNode>(nameToken.value);
    
    // 解析参数
    if (match(CHTL_TokenType::PAREN_OPEN)) {
        consume(); // 消费 (
        
        while (!match(CHTL_TokenType::PAREN_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            auto arg = parseExpression();
            if (arg) {
                useNode->addArgument(arg);
            }
            
            if (match(CHTL_TokenType::OPERATOR_COMMA)) {
                consume(); // 消费逗号
            }
        }
        
        expect(CHTL_TokenType::PAREN_CLOSE, "Expected ')'");
    }
    
    expect(CHTL_TokenType::OPERATOR_SEMICOLON, "Expected ';'");
    
    return useNode;
}

std::unique_ptr<CHTLImportNode> CHTLParser::parseImportStatement() {
    expect(CHTL_TokenType::KEYWORD_IMPORT, "Expected 'import'");
    
    // 解析导入项
    std::vector<std::string> importedItems;
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token itemToken = consume();
            if (itemToken.type == CHTL_TokenType::IDENTIFIER) {
                importedItems.push_back(itemToken.value);
            }
            
            if (match(CHTL_TokenType::OPERATOR_COMMA)) {
                consume(); // 消费逗号
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    expect(CHTL_TokenType::KEYWORD_FROM, "Expected 'from'");
    
    CHTL_Token pathToken = consume();
    if (pathToken.type != CHTL_TokenType::STRING_LITERAL) {
        throw std::runtime_error("Expected import path");
    }
    
    auto importNode = std::make_unique<CHTLImportNode>(pathToken.value);
    
    // 添加导入项
    for (const auto& item : importedItems) {
        importNode->addImportedItem(item);
    }
    
    expect(CHTL_TokenType::OPERATOR_SEMICOLON, "Expected ';'");
    
    return importNode;
}

std::unique_ptr<CHTLNamespaceNode> CHTLParser::parseNamespaceDefinition() {
    expect(CHTL_TokenType::KEYWORD_NAMESPACE, "Expected 'namespace'");
    
    CHTL_Token nameToken = consume();
    if (nameToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected namespace name");
    }
    
    auto namespaceNode = std::make_unique<CHTLNamespaceNode>(nameToken.value);
    
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            auto bodyNode = parseStatement();
            if (bodyNode) {
                namespaceNode->addBodyNode(bodyNode);
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return namespaceNode;
}

std::unique_ptr<CHTLConfigNode> CHTLParser::parseConfigDefinition() {
    expect(CHTL_TokenType::KEYWORD_CONFIG, "Expected 'config'");
    
    CHTL_Token nameToken = consume();
    if (nameToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected config name");
    }
    
    auto configNode = std::make_unique<CHTLConfigNode>(nameToken.value);
    
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        
        while (!match(CHTL_TokenType::BRACE_CLOSE) && currentTokenIndex_ < tokens_.size()) {
            CHTL_Token keyToken = consume();
            if (keyToken.type == CHTL_TokenType::IDENTIFIER) {
                expect(CHTL_TokenType::OPERATOR_EQUAL, "Expected '='");
                
                auto value = parseExpression();
                if (value) {
                    configNode->addProperty(keyToken.value, value);
                }
                
                expect(CHTL_TokenType::OPERATOR_SEMICOLON, "Expected ';'");
            }
        }
        
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    return configNode;
}

std::unique_ptr<CHTLDeleteNode> CHTLParser::parseDeleteOperator() {
    expect(CHTL_TokenType::KEYWORD_DELETE, "Expected 'delete'");
    
    CHTL_Token targetToken = consume();
    if (targetToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected delete target");
    }
    
    // 确定删除目标类型
    CHTLDeleteNode::DeleteTargetType targetType = CHTLDeleteNode::DeleteTargetType::UNKNOWN;
    if (targetToken.value == "attribute") {
        targetType = CHTLDeleteNode::DeleteTargetType::ATTRIBUTE;
    } else if (targetToken.value == "element") {
        targetType = CHTLDeleteNode::DeleteTargetType::CHILD_ELEMENT;
    } else if (targetToken.value == "property") {
        targetType = CHTLDeleteNode::DeleteTargetType::STYLE_PROPERTY;
    } else if (targetToken.value == "group") {
        targetType = CHTLDeleteNode::DeleteTargetType::STYLE_GROUP;
    } else if (targetToken.value == "inheritance") {
        targetType = CHTLDeleteNode::DeleteTargetType::INHERITANCE;
    }
    
    expect(CHTL_TokenType::OPERATOR_SEMICOLON, "Expected ';'");
    
    return std::make_unique<CHTLDeleteNode>(targetType, targetToken.value);
}

std::unique_ptr<CHTLInsertNode> CHTLParser::parseInsertOperator() {
    expect(CHTL_TokenType::KEYWORD_INSERT, "Expected 'insert'");
    
    CHTL_Token positionToken = consume();
    if (positionToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected insert position");
    }
    
    CHTL_Token targetToken = consume();
    if (targetToken.type != CHTL_TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected insert target");
    }
    
    // 解析插入内容
    std::unique_ptr<CHTLBaseNode> content = nullptr;
    if (match(CHTL_TokenType::BRACE_OPEN)) {
        consume(); // 消费 {
        content = parseStatement();
        expect(CHTL_TokenType::BRACE_CLOSE, "Expected '}'");
    }
    
    expect(CHTL_TokenType::OPERATOR_SEMICOLON, "Expected ';'");
    
    return std::make_unique<CHTLInsertNode>(CHTLInsertNode::InsertTargetType::CHILD_ELEMENT, 
                                           content, positionToken.value);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseCHTLSpecialSyntax() {
    CHTL_Token token = peek();
    
    switch (token.type) {
        case CHTL_TokenType::CHTL_TEMPLATE_VARIABLE:
            return parseTemplateVariable();
        case CHTL_TokenType::CHTL_CUSTOM_VARIABLE:
            return parseCustomVariable();
        case CHTL_TokenType::CHTL_CUSTOM_VARIABLE_SPECIALIZATION:
            return parseCustomVariableSpecialization();
        case CHTL_TokenType::CHTL_TEMPLATE_STYLE_GROUP:
        case CHTL_TokenType::CHTL_CUSTOM_STYLE_GROUP:
        case CHTL_TokenType::CHTL_CUSTOM_STYLE_GROUP_SPECIALIZATION:
        case CHTL_TokenType::CHTL_NO_VALUE_STYLE_GROUP:
            return parseStyleGroupReference();
        case CHTL_TokenType::CHTL_RAW_EMBED:
            return parseRawEmbed();
        case CHTL_TokenType::CHTL_JS_SPECIAL_SYNTAX:
            return parseCHTLJSSpecialSyntax();
        default:
            return nullptr;
    }
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseTemplateVariable() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::TEMPLATE_VARIABLE);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseCustomVariable() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::CUSTOM_VARIABLE);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseCustomVariableSpecialization() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::CUSTOM_VARIABLE_SPECIALIZATION);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseStyleGroupReference() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::TEMPLATE_STYLE_GROUP);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseRawEmbed() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::RAW_EMBED);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseCHTLJSSpecialSyntax() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::CHTL_JS_SPECIAL_SYNTAX);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseExpression() {
    return parsePrimaryExpression();
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parsePrimaryExpression() {
    CHTL_Token token = peek();
    
    switch (token.type) {
        case CHTL_TokenType::STRING_LITERAL:
        case CHTL_TokenType::UNQUOTED_LITERAL:
        case CHTL_TokenType::NUMBER_LITERAL:
            consume();
            return std::make_unique<CHTLBaseNode>(CHTL_NodeType::LITERAL);
        case CHTL_TokenType::IDENTIFIER:
            consume();
            return std::make_unique<CHTLBaseNode>(CHTL_NodeType::IDENTIFIER);
        case CHTL_TokenType::PAREN_OPEN:
            consume(); // 消费 (
            auto expr = parseExpression();
            expect(CHTL_TokenType::PAREN_CLOSE, "Expected ')'");
            return expr;
        default:
            return nullptr;
    }
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseLiteral() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::LITERAL);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseIdentifier() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLBaseNode>(CHTL_NodeType::IDENTIFIER);
}

std::unique_ptr<CHTLBaseNode> CHTLParser::parseGeneratorComment() {
    CHTL_Token token = consume();
    return std::make_unique<CHTLCommentNode>(token.value, token.type);
}

} // namespace CHTL