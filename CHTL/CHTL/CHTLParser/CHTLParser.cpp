#include "CHTLParser.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser(const CHTLTokenList& tokenList) 
    : tokens(tokenList), currentIndex(0) {}

CHTLParser::CHTLParser(const std::string& source) 
    : currentIndex(0) {
    CHTLLexer lexer(source);
    tokens = lexer.tokenize();
}

CHTLToken CHTLParser::getCurrent() const {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex];
    }
    return CHTLToken(CHTLTokenType::EOF_TOKEN);
}

CHTLToken CHTLParser::peek(size_t offset) const {
    size_t index = currentIndex + offset - 1;
    if (index < tokens.size()) {
        return tokens[index];
    }
    return CHTLToken(CHTLTokenType::EOF_TOKEN);
}

bool CHTLParser::hasNext() const {
    return currentIndex < tokens.size();
}

bool CHTLParser::hasNext(size_t offset) const {
    return (currentIndex + offset - 1) < tokens.size();
}

void CHTLParser::advance() {
    if (currentIndex < tokens.size()) {
        currentIndex++;
    }
}

bool CHTLParser::match(CHTLTokenType type) const {
    if (isAtEnd()) return false;
    return getCurrent().type == type;
}

bool CHTLParser::match(const std::vector<CHTLTokenType>& types) const {
    if (isAtEnd()) return false;
    CHTLTokenType currentType = getCurrent().type;
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

CHTLToken CHTLParser::consume(CHTLTokenType type, const std::string& message) {
    if (match(type)) {
        CHTLToken token = getCurrent();
        advance();
        return token;
    }
    error(message);
    return CHTLToken();
}

bool CHTLParser::check(CHTLTokenType type) const {
    if (isAtEnd()) return false;
    return getCurrent().type == type;
}

bool CHTLParser::isAtEnd() const {
    return getCurrent().type == CHTLTokenType::EOF_TOKEN;
}

void CHTLParser::error(const std::string& message) const {
    CHTLToken token = getCurrent();
    std::ostringstream oss;
    oss << "Parser error at line " << token.line 
        << ", column " << token.column << ": " << message;
    throw std::runtime_error(oss.str());
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (getCurrent().type == CHTLTokenType::SEMICOLON) {
            advance();
            break;
        }
        
        switch (getCurrent().type) {
            case CHTLTokenType::HTML_ELEMENT:
            case CHTLTokenType::TEMPLATE:
            case CHTLTokenType::CUSTOM:
            case CHTLTokenType::ORIGIN:
            case CHTLTokenType::IMPORT:
            case CHTLTokenType::NAMESPACE:
            case CHTLTokenType::CONFIGURATION:
            case CHTLTokenType::INFO:
            case CHTLTokenType::EXPORT:
                return;
            default:
                advance();
                break;
        }
    }
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseProgram() {
    auto program = std::make_shared<ProgramNode>();
    
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program->addChild(statement);
            }
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return program;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStatement() {
    if (match(CHTLTokenType::HTML_ELEMENT)) {
        return parseHTMLElement();
    }
    
    if (match(CHTLTokenType::TEMPLATE)) {
        return parseTemplateDeclaration();
    }
    
    if (match(CHTLTokenType::CUSTOM)) {
        return parseCustomDeclaration();
    }
    
    if (match(CHTLTokenType::ORIGIN)) {
        return parseOriginNode();
    }
    
    if (match(CHTLTokenType::IMPORT)) {
        return parseImportNode();
    }
    
    if (match(CHTLTokenType::NAMESPACE)) {
        return parseNamespaceNode();
    }
    
    if (match(CHTLTokenType::CONFIGURATION)) {
        return parseConfigurationNode();
    }
    
    if (match(CHTLTokenType::INFO)) {
        return parseInfoNode();
    }
    
    if (match(CHTLTokenType::EXPORT)) {
        return parseExportNode();
    }
    
    if (match(CHTLTokenType::KEYWORD_DELETE)) {
        return parseDeleteNode();
    }
    
    if (match(CHTLTokenType::KEYWORD_INSERT)) {
        return parseInsertNode();
    }
    
    if (match(CHTLTokenType::KEYWORD_INHERIT)) {
        return parseInheritNode();
    }
    
    if (match(CHTLTokenType::KEYWORD_EXCEPT)) {
        return parseExceptNode();
    }
    
    // 默认作为文本节点处理
    return parseTextNode();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseHTMLElement() {
    CHTLToken elementToken = consume(CHTLTokenType::HTML_ELEMENT, "Expected HTML element");
    auto element = std::make_shared<HTMLElementNode>(elementToken.value);
    
    // 检查是否是自闭合元素
    element->isSelfClosing = isSelfClosingElement(elementToken.value);
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    // 解析属性
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(CHTLTokenType::IDENTIFIER)) {
            auto attr = parseAttribute();
            if (attr) {
                element->attributes.push_back(attr);
            }
        } else if (match(CHTLTokenType::KEYWORD_STYLE)) {
            advance(); // 跳过 'style'
            consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
            auto styleBlock = parseStyleBlock();
            if (styleBlock) {
                element->addChild(styleBlock);
            }
        } else if (match(CHTLTokenType::KEYWORD_SCRIPT)) {
            advance(); // 跳过 'script'
            consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
            auto scriptBlock = parseScriptBlock();
            if (scriptBlock) {
                element->addChild(scriptBlock);
            }
        } else if (match(CHTLTokenType::KEYWORD_TEXT)) {
            advance(); // 跳过 'text'
            consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
            auto textNode = parseTextNode();
            if (textNode) {
                element->addChild(textNode);
            }
        } else if (match(CHTLTokenType::HTML_ELEMENT)) {
            auto childElement = parseHTMLElement();
            if (childElement) {
                element->addChild(childElement);
            }
        } else {
            advance(); // 跳过未知token
        }
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return element;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTextNode() {
    std::string content;
    
    while (!isAtEnd() && !check(CHTLTokenType::RIGHT_BRACE)) {
        CHTLToken token = getCurrent();
        
        if (token.type == CHTLTokenType::STRING) {
            content += token.value;
        } else if (token.type == CHTLTokenType::IDENTIFIER) {
            content += token.value;
        } else if (token.type == CHTLTokenType::NUMBER) {
            content += token.value;
        } else if (token.type == CHTLTokenType::BOOLEAN) {
            content += token.value;
        } else if (token.type == CHTLTokenType::WHITESPACE) {
            content += " ";
        } else if (token.type == CHTLTokenType::NEWLINE) {
            content += "\n";
        } else {
            break;
        }
        
        advance();
    }
    
    if (!content.empty()) {
        return std::make_shared<TextNode>(content);
    }
    
    return nullptr;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseAttribute() {
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected attribute name");
    auto attr = std::make_shared<AttributeNode>(nameToken.value);
    
    if (match(CHTLTokenType::COLON) || match(CHTLTokenType::ASSIGN)) {
        advance(); // 跳过 ':' 或 '='
        
        if (match(CHTLTokenType::STRING)) {
            CHTLToken valueToken = consume(CHTLTokenType::STRING, "Expected string value");
            attr->value = valueToken.value;
            attr->hasValue = true;
        } else if (match(CHTLTokenType::IDENTIFIER)) {
            CHTLToken valueToken = consume(CHTLTokenType::IDENTIFIER, "Expected identifier value");
            attr->value = valueToken.value;
            attr->hasValue = true;
        } else if (match(CHTLTokenType::NUMBER)) {
            CHTLToken valueToken = consume(CHTLTokenType::NUMBER, "Expected number value");
            attr->value = valueToken.value;
            attr->hasValue = true;
        } else if (match(CHTLTokenType::BOOLEAN)) {
            CHTLToken valueToken = consume(CHTLTokenType::BOOLEAN, "Expected boolean value");
            attr->value = valueToken.value;
            attr->hasValue = true;
        }
    }
    
    return attr;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleBlock() {
    auto styleBlock = std::make_shared<StyleBlockNode>(false); // 局部样式块
    
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto rule = parseStyleRule();
        if (rule) {
            styleBlock->rules.push_back(rule);
        }
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return styleBlock;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleRule() {
    auto rule = std::make_shared<StyleRuleNode>();
    
    // 解析选择器
    if (match(CHTLTokenType::DOT)) {
        advance(); // 跳过 '.'
        CHTLToken classToken = consume(CHTLTokenType::IDENTIFIER, "Expected class name");
        rule->selector = std::make_shared<TextNode>("." + classToken.value);
    } else if (match(CHTLTokenType::HASH)) {
        advance(); // 跳过 '#'
        CHTLToken idToken = consume(CHTLTokenType::IDENTIFIER, "Expected id name");
        rule->selector = std::make_shared<TextNode>("#" + idToken.value);
    } else if (match(CHTLTokenType::AMPERSAND)) {
        advance(); // 跳过 '&'
        rule->selector = std::make_shared<TextNode>("&");
    } else {
        rule->selector = std::make_shared<TextNode>("*");
    }
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    // 解析样式属性
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto prop = parseStyleProperty();
        if (prop) {
            rule->properties.push_back(prop);
        }
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return rule;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleProperty() {
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected property name");
    consume(CHTLTokenType::COLON, "Expected ':'");
    
    std::string value;
    while (!isAtEnd() && !check(CHTLTokenType::SEMICOLON) && !check(CHTLTokenType::RIGHT_BRACE)) {
        CHTLToken token = getCurrent();
        if (token.type == CHTLTokenType::STRING) {
            value += token.value;
        } else if (token.type == CHTLTokenType::IDENTIFIER) {
            value += token.value;
        } else if (token.type == CHTLTokenType::NUMBER) {
            value += token.value;
        } else if (token.type == CHTLTokenType::PLUS || token.type == CHTLTokenType::MINUS ||
                   token.type == CHTLTokenType::MULTIPLY || token.type == CHTLTokenType::DIVIDE ||
                   token.type == CHTLTokenType::POWER) {
            value += getOperatorString(token.type);
        } else if (token.type == CHTLTokenType::WHITESPACE) {
            value += " ";
        } else {
            break;
        }
        advance();
    }
    
    if (match(CHTLTokenType::SEMICOLON)) {
        advance();
    }
    
    return std::make_shared<StylePropertyNode>(nameToken.value, value);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseScriptBlock() {
    std::string content;
    
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        CHTLToken token = getCurrent();
        content += token.value;
        advance();
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return std::make_shared<ScriptBlockNode>(content);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplateDeclaration() {
    consume(CHTLTokenType::TEMPLATE, "Expected '[Template]'");
    
    CHTLToken typeToken = consume(CHTLTokenType::TEMPLATE_STYLE, "Expected template type");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected template name");
    
    auto templateDecl = std::make_shared<TemplateDeclarationNode>(typeToken.value, nameToken.value);
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto content = parseStatement();
        if (content) {
            templateDecl->content.push_back(content);
        }
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return templateDecl;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseCustomDeclaration() {
    consume(CHTLTokenType::CUSTOM, "Expected '[Custom]'");
    
    CHTLToken typeToken = consume(CHTLTokenType::TEMPLATE_STYLE, "Expected custom type");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected custom name");
    
    auto customDecl = std::make_shared<CustomDeclarationNode>(typeToken.value, nameToken.value);
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto content = parseStatement();
        if (content) {
            customDecl->content.push_back(content);
        }
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return customDecl;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseOriginNode() {
    consume(CHTLTokenType::ORIGIN, "Expected '[Origin]'");
    
    CHTLToken typeToken = consume(CHTLTokenType::ORIGIN_HTML, "Expected origin type");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected origin name");
    
    auto originNode = std::make_shared<OriginNode>(typeToken.value, nameToken.value);
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    std::string content;
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        CHTLToken token = getCurrent();
        content += token.value;
        advance();
    }
    
    originNode->content = content;
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return originNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseImportNode() {
    consume(CHTLTokenType::IMPORT, "Expected '[Import]'");
    
    CHTLToken typeToken = consume(CHTLTokenType::IMPORT_CHTL, "Expected import type");
    CHTLToken pathToken = consume(CHTLTokenType::STRING, "Expected import path");
    
    auto importNode = std::make_shared<ImportNode>(typeToken.value, pathToken.value);
    
    if (match(CHTLTokenType::KEYWORD_AS)) {
        advance(); // 跳过 'as'
        CHTLToken aliasToken = consume(CHTLTokenType::IDENTIFIER, "Expected alias");
        importNode->alias = aliasToken.value;
    }
    
    if (match(CHTLTokenType::KEYWORD_FROM)) {
        advance(); // 跳过 'from'
        CHTLToken fromToken = consume(CHTLTokenType::STRING, "Expected from path");
        importNode->from = fromToken.value;
    }
    
    return importNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseNamespaceNode() {
    consume(CHTLTokenType::NAMESPACE, "Expected '[Namespace]'");
    
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected namespace name");
    auto namespaceNode = std::make_shared<NamespaceNode>(nameToken.value);
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
        
        while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto content = parseStatement();
            if (content) {
                namespaceNode->content.push_back(content);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return namespaceNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseConfigurationNode() {
    consume(CHTLTokenType::CONFIGURATION, "Expected '[Configuration]'");
    
    auto configNode = std::make_shared<ConfigurationNode>();
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
        
        while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
            CHTLToken keyToken = consume(CHTLTokenType::IDENTIFIER, "Expected config key");
            consume(CHTLTokenType::ASSIGN, "Expected '='");
            CHTLToken valueToken = consume(CHTLTokenType::STRING, "Expected config value");
            
            configNode->settings[keyToken.value] = valueToken.value;
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return configNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseInfoNode() {
    consume(CHTLTokenType::INFO, "Expected '[Info]'");
    
    auto infoNode = std::make_shared<InfoNode>();
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        CHTLToken keyToken = consume(CHTLTokenType::IDENTIFIER, "Expected info key");
        consume(CHTLTokenType::ASSIGN, "Expected '='");
        CHTLToken valueToken = consume(CHTLTokenType::STRING, "Expected info value");
        
        infoNode->info[keyToken.value] = valueToken.value;
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return infoNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseExportNode() {
    consume(CHTLTokenType::EXPORT, "Expected '[Export]'");
    
    auto exportNode = std::make_shared<ExportNode>();
    
    consume(CHTLTokenType::LEFT_BRACE, "Expected '{'");
    
    while (!check(CHTLTokenType::RIGHT_BRACE) && !isAtEnd()) {
        CHTLToken exportToken = consume(CHTLTokenType::IDENTIFIER, "Expected export name");
        exportNode->exports.push_back(exportToken.value);
        
        if (match(CHTLTokenType::COMMA)) {
            advance();
        }
    }
    
    consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    return exportNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseDeleteNode() {
    consume(CHTLTokenType::KEYWORD_DELETE, "Expected 'delete'");
    
    auto deleteNode = std::make_shared<CHTLASTNode>(CHTLASTNodeType::DELETE_NODE);
    
    while (!isAtEnd() && !check(CHTLTokenType::SEMICOLON)) {
        CHTLToken token = getCurrent();
        if (token.type == CHTLTokenType::IDENTIFIER) {
            auto child = std::make_shared<TextNode>(token.value);
            deleteNode->addChild(child);
        }
        advance();
    }
    
    if (match(CHTLTokenType::SEMICOLON)) {
        advance();
    }
    
    return deleteNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseInsertNode() {
    consume(CHTLTokenType::KEYWORD_INSERT, "Expected 'insert'");
    
    auto insertNode = std::make_shared<CHTLASTNode>(CHTLASTNodeType::INSERT_NODE);
    
    // 解析插入位置和内容
    while (!isAtEnd() && !check(CHTLTokenType::SEMICOLON)) {
        CHTLToken token = getCurrent();
        if (token.type == CHTLTokenType::IDENTIFIER) {
            auto child = std::make_shared<TextNode>(token.value);
            insertNode->addChild(child);
        }
        advance();
    }
    
    if (match(CHTLTokenType::SEMICOLON)) {
        advance();
    }
    
    return insertNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseInheritNode() {
    consume(CHTLTokenType::KEYWORD_INHERIT, "Expected 'inherit'");
    
    auto inheritNode = std::make_shared<CHTLASTNode>(CHTLASTNodeType::INHERIT_NODE);
    
    while (!isAtEnd() && !check(CHTLTokenType::SEMICOLON)) {
        CHTLToken token = getCurrent();
        if (token.type == CHTLTokenType::IDENTIFIER) {
            auto child = std::make_shared<TextNode>(token.value);
            inheritNode->addChild(child);
        }
        advance();
    }
    
    if (match(CHTLTokenType::SEMICOLON)) {
        advance();
    }
    
    return inheritNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseExceptNode() {
    consume(CHTLTokenType::KEYWORD_EXCEPT, "Expected 'except'");
    
    auto exceptNode = std::make_shared<CHTLASTNode>(CHTLASTNodeType::EXCEPT_NODE);
    
    while (!isAtEnd() && !check(CHTLTokenType::SEMICOLON)) {
        CHTLToken token = getCurrent();
        if (token.type == CHTLTokenType::IDENTIFIER) {
            auto child = std::make_shared<TextNode>(token.value);
            exceptNode->addChild(child);
        }
        advance();
    }
    
    if (match(CHTLTokenType::SEMICOLON)) {
        advance();
    }
    
    return exceptNode;
}

std::vector<std::shared_ptr<CHTLASTNode>> CHTLParser::parseAttributeList() {
    std::vector<std::shared_ptr<CHTLASTNode>> attributes;
    
    while (!isAtEnd() && !check(CHTLTokenType::RIGHT_BRACE)) {
        if (match(CHTLTokenType::IDENTIFIER)) {
            auto attr = parseAttribute();
            if (attr) {
                attributes.push_back(attr);
            }
        } else {
            advance();
        }
    }
    
    return attributes;
}

std::vector<std::shared_ptr<CHTLASTNode>> CHTLParser::parseStyleRules() {
    std::vector<std::shared_ptr<CHTLASTNode>> rules;
    
    while (!isAtEnd() && !check(CHTLTokenType::RIGHT_BRACE)) {
        auto rule = parseStyleRule();
        if (rule) {
            rules.push_back(rule);
        }
    }
    
    return rules;
}

std::vector<std::shared_ptr<CHTLASTNode>> CHTLParser::parseStyleProperties() {
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
    
    while (!isAtEnd() && !check(CHTLTokenType::RIGHT_BRACE)) {
        auto prop = parseStyleProperty();
        if (prop) {
            properties.push_back(prop);
        }
    }
    
    return properties;
}

std::string CHTLParser::parseStringLiteral() {
    CHTLToken token = consume(CHTLTokenType::STRING, "Expected string literal");
    return token.value;
}

std::string CHTLParser::parseUnquotedLiteral() {
    CHTLToken token = consume(CHTLTokenType::IDENTIFIER, "Expected unquoted literal");
    return token.value;
}

bool CHTLParser::isHTMLElement(const std::string& name) const {
    // 简化的HTML元素检查
    static const std::vector<std::string> htmlElements = {
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "form", "input",
        "button", "textarea", "select", "option", "label", "fieldset", "legend"
    };
    
    return std::find(htmlElements.begin(), htmlElements.end(), name) != htmlElements.end();
}

bool CHTLParser::isSelfClosingElement(const std::string& name) const {
    static const std::vector<std::string> selfClosingElements = {
        "img", "br", "hr", "input", "meta", "link", "area", "base", "col", "embed",
        "source", "track", "wbr"
    };
    
    return std::find(selfClosingElements.begin(), selfClosingElements.end(), name) != selfClosingElements.end();
}

std::string CHTLParser::getOperatorString(CHTLTokenType type) const {
    switch (type) {
        case CHTLTokenType::PLUS: return "+";
        case CHTLTokenType::MINUS: return "-";
        case CHTLTokenType::MULTIPLY: return "*";
        case CHTLTokenType::DIVIDE: return "/";
        case CHTLTokenType::POWER: return "**";
        case CHTLTokenType::MODULO: return "%";
        default: return "";
    }
}

std::shared_ptr<CHTLASTNode> CHTLParser::parse() {
    return parseProgram();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parse(const std::string& source) {
    CHTLLexer lexer(source);
    tokens = lexer.tokenize();
    currentIndex = 0;
    return parseProgram();
}

void CHTLParser::reset() {
    currentIndex = 0;
}

void CHTLParser::setTokens(const CHTLTokenList& tokenList) {
    tokens = tokenList;
    currentIndex = 0;
}

CHTLTokenList CHTLParser::getTokens() const {
    return tokens;
}

size_t CHTLParser::getCurrentIndex() const {
    return currentIndex;
}

bool CHTLParser::hasErrors() const {
    return false; // 简化实现
}

} // namespace CHTL