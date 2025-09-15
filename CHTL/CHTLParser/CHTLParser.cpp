#include "CHTLParser.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser() : position(0) {
}

CHTLParser::~CHTLParser() {
}

std::shared_ptr<ProgramNode> CHTLParser::parse(const std::vector<CHTLToken>& tokens) {
    setTokens(tokens);
    auto program = std::make_shared<ProgramNode>();
    
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program->addStatement(statement);
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return program;
}

void CHTLParser::setTokens(const std::vector<CHTLToken>& tokens) {
    this->tokens = tokens;
    reset();
}

void CHTLParser::reset() {
    position = 0;
    errors.clear();
    templates.clear();
    customs.clear();
    variables.clear();
    currentNamespace.clear();
    globalStyles.clear();
}

bool CHTLParser::hasMoreTokens() const {
    return position < tokens.size();
}

CHTLToken CHTLParser::currentToken() const {
    if (isAtEnd()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN, "", 0, 0);
    }
    return tokens[position];
}

CHTLToken CHTLParser::peekToken() const {
    if (position + 1 >= tokens.size()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN, "", 0, 0);
    }
    return tokens[position + 1];
}

void CHTLParser::advance() {
    if (!isAtEnd()) {
        position++;
    }
}

std::vector<std::string> CHTLParser::getErrors() const {
    return errors;
}

bool CHTLParser::hasErrors() const {
    return !errors.empty();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStatement() {
    if (isAtEnd()) {
        return nullptr;
    }
    
    CHTLToken token = currentToken();
    
    // 使用语句
    if (token.getType() == CHTLTokenType::USE) {
        return parseUseStatement();
    }
    
    // 模板声明
    if (token.getType() == CHTLTokenType::TEMPLATE) {
        return parseTemplate();
    }
    
    // 自定义声明
    if (token.getType() == CHTLTokenType::CUSTOM) {
        return parseCustom();
    }
    
    // 原始嵌入
    if (token.getType() == CHTLTokenType::ORIGIN) {
        return parseOrigin();
    }
    
    // 导入
    if (token.getType() == CHTLTokenType::IMPORT) {
        return parseImport();
    }
    
    // 配置
    if (token.getType() == CHTLTokenType::CONFIGURATION) {
        return parseConfiguration();
    }
    
    // 命名空间
    if (token.getType() == CHTLTokenType::NAMESPACE) {
        return parseNamespace();
    }
    
    // 信息
    if (token.getType() == CHTLTokenType::INFO) {
        return parseInfo();
    }
    
    // 导出
    if (token.getType() == CHTLTokenType::EXPORT) {
        return parseExport();
    }
    
    // 文本节点
    if (token.getType() == CHTLTokenType::TEXT) {
        return parseText();
    }
    
    // HTML元素
    if (token.isHTMLTag()) {
        return parseElement();
    }
    
    // 标识符（可能是元素）
    if (token.getType() == CHTLTokenType::IDENTIFIER) {
        return parseElement();
    }
    
    reportError("Unexpected token: " + token.getValue());
    return nullptr;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseElement() {
    CHTLToken nameToken = currentToken();
    advance();
    
    auto element = std::make_shared<ElementNode>(nameToken.getValue());
    
    // 解析属性
    while (!isAtEnd() && currentToken().getType() != CHTLTokenType::LEFT_BRACE) {
        if (currentToken().getType() == CHTLTokenType::IDENTIFIER) {
            auto attribute = parseAttribute();
            if (attribute) {
                element->addAttribute(attribute);
            }
        } else {
            break;
        }
    }
    
    // 解析内容
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto child = parseStatement();
            if (child) {
                element->addChild(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return element;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseText() {
    CHTLToken textToken = consume(CHTLTokenType::TEXT, "Expected 'text'");
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        std::string content;
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            content += currentToken().getValue();
            advance();
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
        return std::make_shared<TextNode>(content);
    } else if (currentToken().getType() == CHTLTokenType::COLON) {
        advance(); // 跳过 ':'
        auto value = parseExpression();
        if (value) {
            return std::make_shared<TextNode>(value->generateCode());
        }
    }
    
    return std::make_shared<TextNode>("");
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseAttribute() {
    CHTLToken nameToken = currentToken();
    advance();
    
    std::string name = nameToken.getValue();
    std::shared_ptr<CHTLASTNode> value = nullptr;
    
    if (currentToken().getType() == CHTLTokenType::COLON || 
        currentToken().getType() == CHTLTokenType::EQUAL) {
        advance(); // 跳过 ':' 或 '='
        value = parseExpression();
    }
    
    return std::make_shared<AttributeNode>(name, value);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleBlock() {
    CHTLToken styleToken = consume(CHTLTokenType::STYLE, "Expected 'style'");
    auto styleBlock = std::make_shared<StyleBlockNode>();
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto rule = parseStyleRule();
            if (rule) {
                styleBlock->addRule(rule);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return styleBlock;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseScriptBlock() {
    CHTLToken scriptToken = consume(CHTLTokenType::SCRIPT, "Expected 'script'");
    
    std::string content;
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            content += currentToken().getValue();
            advance();
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return std::make_shared<ScriptBlockNode>(content);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplate() {
    CHTLToken templateToken = consume(CHTLTokenType::TEMPLATE, "Expected '[Template]'");
    
    CHTLToken typeToken = currentToken();
    if (typeToken.getType() == CHTLTokenType::TEMPLATE_STYLE) {
        return parseTemplateStyle();
    } else if (typeToken.getType() == CHTLTokenType::TEMPLATE_ELEMENT) {
        return parseTemplateElement();
    } else if (typeToken.getType() == CHTLTokenType::TEMPLATE_VAR) {
        return parseTemplateVar();
    }
    
    reportError("Expected template type (@Style, @Element, @Var)");
    return nullptr;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplateStyle() {
    CHTLToken typeToken = consume(CHTLTokenType::TEMPLATE_STYLE, "Expected '@Style'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected template name");
    
    auto template_ = std::make_shared<TemplateStyleNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                template_->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    addTemplate(nameToken.getValue(), template_);
    return template_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplateElement() {
    CHTLToken typeToken = consume(CHTLTokenType::TEMPLATE_ELEMENT, "Expected '@Element'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected template name");
    
    auto template_ = std::make_shared<TemplateElementNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto child = parseStatement();
            if (child) {
                template_->addChild(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    addTemplate(nameToken.getValue(), template_);
    return template_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplateVar() {
    CHTLToken typeToken = consume(CHTLTokenType::TEMPLATE_VAR, "Expected '@Var'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected template name");
    
    auto template_ = std::make_shared<TemplateVarNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                template_->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    addTemplate(nameToken.getValue(), template_);
    return template_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseCustom() {
    CHTLToken customToken = consume(CHTLTokenType::CUSTOM, "Expected '[Custom]'");
    
    CHTLToken typeToken = currentToken();
    if (typeToken.getType() == CHTLTokenType::CUSTOM_STYLE) {
        return parseCustomStyle();
    } else if (typeToken.getType() == CHTLTokenType::CUSTOM_ELEMENT) {
        return parseCustomElement();
    } else if (typeToken.getType() == CHTLTokenType::CUSTOM_VAR) {
        return parseCustomVar();
    }
    
    reportError("Expected custom type (@Style, @Element, @Var)");
    return nullptr;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseCustomStyle() {
    CHTLToken typeToken = consume(CHTLTokenType::CUSTOM_STYLE, "Expected '@Style'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected custom name");
    
    auto custom = std::make_shared<CustomStyleNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                custom->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    addCustom(nameToken.getValue(), custom);
    return custom;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseCustomElement() {
    CHTLToken typeToken = consume(CHTLTokenType::CUSTOM_ELEMENT, "Expected '@Element'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected custom name");
    
    auto custom = std::make_shared<CustomElementNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto child = parseStatement();
            if (child) {
                custom->addChild(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    addCustom(nameToken.getValue(), custom);
    return custom;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseCustomVar() {
    CHTLToken typeToken = consume(CHTLTokenType::CUSTOM_VAR, "Expected '@Var'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected custom name");
    
    auto custom = std::make_shared<CustomVarNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                custom->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    addCustom(nameToken.getValue(), custom);
    return custom;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseOrigin() {
    CHTLToken originToken = consume(CHTLTokenType::ORIGIN, "Expected '[Origin]'");
    
    CHTLTokenType type = CHTLTokenType::ORIGIN_HTML;
    if (currentToken().getType() == CHTLTokenType::ORIGIN_HTML) {
        type = CHTLTokenType::ORIGIN_HTML;
    } else if (currentToken().getType() == CHTLTokenType::ORIGIN_STYLE) {
        type = CHTLTokenType::ORIGIN_STYLE;
    } else if (currentToken().getType() == CHTLTokenType::ORIGIN_JAVASCRIPT) {
        type = CHTLTokenType::ORIGIN_JAVASCRIPT;
    }
    
    advance(); // 跳过类型
    
    std::string name;
    if (currentToken().getType() == CHTLTokenType::IDENTIFIER) {
        name = currentToken().getValue();
        advance();
    }
    
    std::string content;
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            content += currentToken().getValue();
            advance();
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return std::make_shared<OriginNode>(static_cast<CHTLASTNode::NodeType>(type), name, content);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseImport() {
    CHTLToken importToken = consume(CHTLTokenType::IMPORT, "Expected '[Import]'");
    
    CHTLTokenType type = CHTLTokenType::IMPORT_CHTL;
    if (currentToken().getType() == CHTLTokenType::IMPORT_HTML) {
        type = CHTLTokenType::IMPORT_HTML;
    } else if (currentToken().getType() == CHTLTokenType::IMPORT_STYLE) {
        type = CHTLTokenType::IMPORT_STYLE;
    } else if (currentToken().getType() == CHTLTokenType::IMPORT_JAVASCRIPT) {
        type = CHTLTokenType::IMPORT_JAVASCRIPT;
    } else if (currentToken().getType() == CHTLTokenType::IMPORT_CHTL) {
        type = CHTLTokenType::IMPORT_CHTL;
    } else if (currentToken().getType() == CHTLTokenType::IMPORT_CJMOD) {
        type = CHTLTokenType::IMPORT_CJMOD;
    }
    
    advance(); // 跳过类型
    
    std::string name;
    if (currentToken().getType() == CHTLTokenType::IDENTIFIER) {
        name = currentToken().getValue();
        advance();
    }
    
    CHTLToken fromToken = consume(CHTLTokenType::FROM, "Expected 'from'");
    CHTLToken pathToken = currentToken();
    advance();
    
    std::string alias;
    if (currentToken().getType() == CHTLTokenType::AS) {
        advance(); // 跳过 'as'
        alias = currentToken().getValue();
        advance();
    }
    
    return std::make_shared<ImportNode>(static_cast<CHTLASTNode::NodeType>(type), name, pathToken.getValue(), alias);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseConfiguration() {
    CHTLToken configToken = consume(CHTLTokenType::CONFIGURATION, "Expected '[Configuration]'");
    
    std::string name;
    if (currentToken().getType() == CHTLTokenType::AT) {
        advance(); // 跳过 '@'
        CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected config name");
        name = nameToken.getValue();
    }
    
    auto config = std::make_shared<ConfigurationNode>(name);
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                config->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return config;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseNamespace() {
    CHTLToken namespaceToken = consume(CHTLTokenType::NAMESPACE, "Expected '[Namespace]'");
    CHTLToken nameToken = consume(CHTLTokenType::IDENTIFIER, "Expected namespace name");
    
    auto namespace_ = std::make_shared<NamespaceNode>(nameToken.getValue());
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto child = parseStatement();
            if (child) {
                namespace_->addChild(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return namespace_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseInfo() {
    CHTLToken infoToken = consume(CHTLTokenType::INFO, "Expected '[Info]'");
    
    auto info = std::make_shared<InfoNode>();
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                info->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return info;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseExport() {
    CHTLToken exportToken = consume(CHTLTokenType::EXPORT, "Expected '[Export]'");
    
    auto export_ = std::make_shared<ExportNode>();
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto child = parseStatement();
            if (child) {
                export_->addExport(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return export_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseUseStatement() {
    CHTLToken useToken = consume(CHTLTokenType::USE, "Expected 'use'");
    CHTLToken targetToken = currentToken();
    advance();
    
    return std::make_shared<UseStatementNode>(targetToken.getValue());
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseExpression() {
    return parseConditionalExpression();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseConditionalExpression() {
    auto left = parseBinaryExpression();
    
    if (currentToken().getType() == CHTLTokenType::QUESTION) {
        advance(); // 跳过 '?'
        auto trueExpr = parseExpression();
        consume(CHTLTokenType::COLON, "Expected ':'");
        auto falseExpr = parseExpression();
        
        return std::make_shared<ConditionalExpressionNode>(left, trueExpr, falseExpr);
    }
    
    return left;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseBinaryExpression() {
    auto left = parsePrimary();
    
    while (currentToken().isOperator()) {
        CHTLToken op = currentToken();
        advance();
        auto right = parsePrimary();
        left = std::make_shared<BinaryExpressionNode>(left, op.getValue(), right);
    }
    
    return left;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parsePrimary() {
    CHTLToken token = currentToken();
    
    switch (token.getType()) {
        case CHTLTokenType::IDENTIFIER:
            return parseIdentifier();
        case CHTLTokenType::STRING:
        case CHTLTokenType::NUMBER:
        case CHTLTokenType::BOOLEAN:
            return parseLiteral();
        case CHTLTokenType::LEFT_PAREN: {
            advance(); // 跳过 '('
            auto expr = parseExpression();
            consume(CHTLTokenType::RIGHT_PAREN, "Expected ')'");
            return expr;
        }
        default:
            reportError("Unexpected token: " + token.getValue());
            return nullptr;
    }
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseLiteral() {
    CHTLToken token = currentToken();
    advance();
    
    std::string type = "string";
    if (token.getType() == CHTLTokenType::NUMBER) {
        type = "number";
    } else if (token.getType() == CHTLTokenType::BOOLEAN) {
        type = "boolean";
    }
    
    return std::make_shared<LiteralNode>(token.getValue(), type);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseIdentifier() {
    CHTLToken token = currentToken();
    advance();
    return std::make_shared<IdentifierNode>(token.getValue());
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleRule() {
    auto selector = parseStyleSelector();
    auto rule = std::make_shared<StyleRuleNode>(selector);
    
    if (currentToken().getType() == CHTLTokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != CHTLTokenType::RIGHT_BRACE) {
            auto property = parseStyleProperty();
            if (property) {
                rule->addProperty(property);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "Expected '}'");
    }
    
    return rule;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleSelector() {
    CHTLToken token = currentToken();
    advance();
    return std::make_shared<StyleSelectorNode>(token.getValue());
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleProperty() {
    CHTLToken nameToken = currentToken();
    advance();
    
    std::shared_ptr<CHTLASTNode> value = nullptr;
    if (currentToken().getType() == CHTLTokenType::COLON) {
        advance(); // 跳过 ':'
        value = parseStyleValue();
    }
    
    return std::make_shared<StylePropertyNode>(nameToken.getValue(), value);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleValue() {
    return parseExpression();
}

bool CHTLParser::match(CHTLTokenType type) const {
    return currentToken().getType() == type;
}

bool CHTLParser::match(const std::vector<CHTLTokenType>& types) const {
    CHTLTokenType current = currentToken().getType();
    return std::find(types.begin(), types.end(), current) != types.end();
}

bool CHTLParser::check(CHTLTokenType type) const {
    return currentToken().getType() == type;
}

CHTLToken CHTLParser::consume(CHTLTokenType type, const std::string& message) {
    if (check(type)) {
        CHTLToken token = currentToken();
        advance();
        return token;
    }
    
    reportError(message);
    return CHTLToken(CHTLTokenType::UNKNOWN, "", 0, 0);
}

CHTLToken CHTLParser::consume(const std::string& message) {
    if (isAtEnd()) {
        reportError(message);
        return CHTLToken(CHTLTokenType::UNKNOWN, "", 0, 0);
    }
    
    CHTLToken token = currentToken();
    advance();
    return token;
}

bool CHTLParser::isAtEnd() const {
    return position >= tokens.size() || currentToken().getType() == CHTLTokenType::EOF_TOKEN;
}

void CHTLParser::reportError(const std::string& message) {
    CHTLToken token = currentToken();
    reportError(token, message);
}

void CHTLParser::reportError(const CHTLToken& token, const std::string& message) {
    std::string error = "Error at line " + std::to_string(token.getLine()) + 
                       ", column " + std::to_string(token.getColumn()) + ": " + message;
    errors.push_back(error);
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (currentToken().getType() == CHTLTokenType::SEMICOLON) {
            advance();
            break;
        }
        
        CHTLTokenType type = currentToken().getType();
        if (type == CHTLTokenType::TEMPLATE || type == CHTLTokenType::CUSTOM ||
            type == CHTLTokenType::ORIGIN || type == CHTLTokenType::IMPORT ||
            type == CHTLTokenType::CONFIGURATION || type == CHTLTokenType::NAMESPACE ||
            type == CHTLTokenType::INFO || type == CHTLTokenType::EXPORT) {
            break;
        }
        
        advance();
    }
}

// 辅助方法实现
std::string CHTLParser::generateGlobalStyles() {
    std::ostringstream oss;
    for (const auto& style : globalStyles) {
        oss << style << "\n";
    }
    return oss.str();
}

void CHTLParser::addGlobalStyle(const std::string& style) {
    globalStyles.push_back(style);
}

bool CHTLParser::isArithmeticOperator(const std::string& op) const {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "**";
}

bool CHTLParser::isComparisonOperator(const std::string& op) const {
    return op == ">" || op == "<" || op == ">=" || op == "<=" || op == "==" || op == "!=";
}

bool CHTLParser::isLogicalOperator(const std::string& op) const {
    return op == "&&" || op == "||";
}

std::string CHTLParser::resolveSelector(const std::string& selector) const {
    // 实现选择器解析逻辑
    return selector;
}

std::string CHTLParser::generateUniqueClass(const std::string& baseName) const {
    // 生成唯一类名
    static int counter = 0;
    return baseName + "_" + std::to_string(++counter);
}

std::string CHTLParser::generateUniqueId(const std::string& baseName) const {
    // 生成唯一ID
    static int counter = 0;
    return baseName + "_" + std::to_string(++counter);
}

std::string CHTLParser::resolveVariable(const std::string& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : name;
}

void CHTLParser::addVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::shared_ptr<CHTLASTNode> CHTLParser::resolveTemplate(const std::string& name) const {
    auto it = templates.find(name);
    return (it != templates.end()) ? it->second : nullptr;
}

void CHTLParser::addTemplate(const std::string& name, std::shared_ptr<CHTLASTNode> template_) {
    templates[name] = template_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::resolveCustom(const std::string& name) const {
    auto it = customs.find(name);
    return (it != customs.end()) ? it->second : nullptr;
}

void CHTLParser::addCustom(const std::string& name, std::shared_ptr<CHTLASTNode> custom) {
    customs[name] = custom;
}

} // namespace CHTL