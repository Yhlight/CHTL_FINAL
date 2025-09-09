#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <sstream>
#include <unordered_set>

namespace CHTL {

CHTLParser::CHTLParser(std::shared_ptr<CHTLLexer> lexer) 
    : m_lexer(lexer), m_currentToken(TokenType::UNKNOWN, ""), m_hasCurrentToken(false) {
}

CHTLParser::~CHTLParser() = default;

std::shared_ptr<BaseNode> CHTLParser::parseDocument() {
    // 创建文档根节点
    auto document = std::make_shared<ElementNode>("html");
    
    // 解析文档内容
    while (true) {
        Token peek = m_lexer->peekToken();
        if (peek.type == TokenType::END_OF_FILE) {
            break;
        }
        Token token = nextToken();
        
        switch (token.type) {
            case TokenType::IDENTIFIER: {
                // 可能是HTML元素
                if (isHTMLElement(token.value)) {
                    auto element = parseElement();
                    if (element) {
                        document->addChild(element);
                    }
                }
                break;
            }
                
            case TokenType::TEXT: {
                // 文本节点
                auto text = parseText();
                if (text) {
                    document->addChild(text);
                }
                break;
            }
                
            case TokenType::LINE_COMMENT:
            case TokenType::BLOCK_COMMENT:
            case TokenType::GENERATOR_COMMENT: {
                // 注释节点
                auto comment = parseComment();
                if (comment) {
                    document->addChild(comment);
                }
                break;
            }
                
            case TokenType::TEMPLATE: {
                // 模板节点
                auto template_node = parseTemplate();
                if (template_node) {
                    document->addChild(template_node);
                }
                break;
            }
                
            case TokenType::CUSTOM: {
                // 自定义节点
                auto custom = parseCustom();
                if (custom) {
                    document->addChild(custom);
                }
                break;
            }
                
            case TokenType::STYLE: {
                // 样式节点
                auto style = parseStyle();
                if (style) {
                    document->addChild(style);
                }
                break;
            }
                
            case TokenType::SCRIPT: {
                // 脚本节点
                auto script = parseScript();
                if (script) {
                    document->addChild(script);
                }
                break;
            }
                
            case TokenType::ORIGIN: {
                // 原始嵌入节点
                auto origin = parseOrigin();
                if (origin) {
                    document->addChild(origin);
                }
                break;
            }
                
            case TokenType::IMPORT: {
                // 导入节点
                auto import = parseImport();
                if (import) {
                    document->addChild(import);
                }
                break;
            }
                
            case TokenType::CONFIGURATION: {
                // 配置节点
                auto config = parseConfig();
                if (config) {
                    document->addChild(config);
                }
                break;
            }
                
            case TokenType::NAMESPACE: {
                // 命名空间节点
                auto namespace_node = parseNamespace();
                if (namespace_node) {
                    document->addChild(namespace_node);
                }
                break;
            }
                
            case TokenType::INHERIT:
            case TokenType::DELETE:
            case TokenType::INSERT:
            case TokenType::REPLACE:
            case TokenType::USE: {
                // 操作符节点
                auto operator_node = parseOperator();
                if (operator_node) {
                    document->addChild(operator_node);
                }
                break;
            }
                
            default:
                // 跳过未知token
                consumeToken();
                break;
        }
    }
    
    return document;
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    // 获取元素标签名
    Token tagToken = nextToken();
    if (tagToken.type != TokenType::IDENTIFIER) {
        addError("Expected element tag name");
        return nullptr;
    }
    
    std::string tagName = tagToken.value;
    auto element = std::make_shared<ElementNode>(tagName);
    
    // 解析属性
    parseAttributes(element);
    
    // 检查是否有子内容
    Token next = peekToken();
    if (next.type == TokenType::LEFT_BRACE) {
        // 有子内容，解析子节点
        consumeToken(); // 消费 '{'
        
        while (true) {
            Token peek = m_lexer->peekToken();
            if (peek.type == TokenType::END_OF_FILE) {
                break;
            }
            Token token = peekToken();
            
            if (token.type == TokenType::RIGHT_BRACE) {
                consumeToken(); // 消费 '}'
                break;
            }
            
            std::shared_ptr<BaseNode> child = nullptr;
            
            switch (token.type) {
                case TokenType::IDENTIFIER: {
                    if (isHTMLElement(token.value)) {
                        child = parseElement();
                    }
                    break;
                }
                    
                case TokenType::TEXT: {
                    child = parseText();
                    break;
                }
                    
                case TokenType::LINE_COMMENT:
                case TokenType::BLOCK_COMMENT:
                case TokenType::GENERATOR_COMMENT: {
                    child = parseComment();
                    break;
                }
                    
                case TokenType::TEMPLATE: {
                    child = parseTemplate();
                    break;
                }
                    
                case TokenType::CUSTOM: {
                    child = parseCustom();
                    break;
                }
                    
                case TokenType::STYLE: {
                    child = parseStyle();
                    break;
                }
                    
                case TokenType::SCRIPT: {
                    child = parseScript();
                    break;
                }
                    
                case TokenType::ORIGIN: {
                    child = parseOrigin();
                    break;
                }
                    
                case TokenType::IMPORT: {
                    child = parseImport();
                    break;
                }
                    
                case TokenType::CONFIGURATION: {
                    child = parseConfig();
                    break;
                }
                    
                case TokenType::NAMESPACE: {
                    child = parseNamespace();
                    break;
                }
                    
                case TokenType::INHERIT:
                case TokenType::DELETE:
                case TokenType::INSERT:
                case TokenType::REPLACE:
                case TokenType::USE: {
                    child = parseOperator();
                    break;
                }
                    
                default:
                    // 跳过未知token
                    consumeToken();
                    break;
            }
            
            if (child) {
                element->addChild(child);
            }
        }
    } else if (next.value == "/") {
        // 自闭合标签
        consumeToken(); // 消费 '/'
        element->setSelfClosing(true);
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    Token token = nextToken();
    if (token.type != TokenType::TEXT) {
        addError("Expected text content");
        return nullptr;
    }
    
    return std::make_shared<TextNode>(token.value);
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    Token token = nextToken();
    CommentType commentType;
    
    if (token.type == TokenType::LINE_COMMENT) {
        commentType = CommentType::LINE;
    } else if (token.type == TokenType::BLOCK_COMMENT) {
        commentType = CommentType::BLOCK;
    } else if (token.type == TokenType::GENERATOR_COMMENT) {
        commentType = CommentType::GENERATOR;
    } else {
        addError("Expected comment");
        return nullptr;
    }
    
    return std::make_shared<CommentNode>(commentType, token.value);
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    // TODO: 实现模板解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    // TODO: 实现自定义解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyle() {
    // TODO: 实现样式解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseScript() {
    // TODO: 实现脚本解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    // TODO: 实现原始嵌入解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    // TODO: 实现导入解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfig() {
    // TODO: 实现配置解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    // TODO: 实现命名空间解析
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOperator() {
    // TODO: 实现操作符解析
    return nullptr;
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> node) {
    while (true) {
        Token peek = m_lexer->peekToken();
        if (peek.type == TokenType::END_OF_FILE) {
            break;
        }
        Token token = peekToken();
        
        // 如果遇到 '{' 或 '/' 或 'EOF'，说明属性解析结束
        if (token.type == TokenType::LEFT_BRACE || token.value == "/" || 
            token.type == TokenType::END_OF_FILE) {
            break;
        }
        
        // 解析属性名
        if (token.type == TokenType::IDENTIFIER) {
            consumeToken(); // 消费属性名
            std::string attrName = token.value;
            
            // 检查是否有属性值
            Token next = peekToken();
            if (next.type == TokenType::COLON || next.type == TokenType::EQUALS) {
                consumeToken(); // 消费 ':' 或 '='
                
                // 解析属性值
                std::string attrValue;
                Token valueToken = peekToken();
                
                if (valueToken.type == TokenType::STRING) {
                    consumeToken();
                    attrValue = valueToken.value;
                } else if (valueToken.type == TokenType::TEXT) {
                    consumeToken();
                    attrValue = valueToken.value;
                } else if (valueToken.type == TokenType::IDENTIFIER) {
                    consumeToken();
                    attrValue = valueToken.value;
                } else {
                    addError("Expected attribute value");
                    break;
                }
                
                // 设置属性
                node->setAttribute(attrName, attrValue);
                
                // 检查是否有分号
                Token semicolon = peekToken();
                if (semicolon.type == TokenType::SEMICOLON) {
                    consumeToken(); // 消费 ';'
                }
            } else {
                // 布尔属性（只有属性名，没有值）
                node->setAttribute(attrName, "");
            }
        } else {
            // 跳过未知token
            consumeToken();
        }
    }
}

void CHTLParser::parseStyleProperties(std::shared_ptr<BaseNode> node) {
    // TODO: 实现样式属性解析
}

void CHTLParser::parseConditionalProperties(std::shared_ptr<BaseNode> node) {
    // TODO: 实现条件属性解析
}

void CHTLParser::parseSelectors(std::shared_ptr<BaseNode> node) {
    // TODO: 实现选择器解析
}

void CHTLParser::parseCHTLJSFunctions(std::shared_ptr<BaseNode> node) {
    // TODO: 实现CHTL JS函数解析
}

void CHTLParser::addError(const std::string& error) {
    m_errors.push_back(error);
}

const std::vector<std::string>& CHTLParser::getErrors() const {
    return m_errors;
}

bool CHTLParser::hasErrors() const {
    return !m_errors.empty();
}

size_t CHTLParser::getCurrentLine() const {
    return m_lexer->getCurrentLine();
}

size_t CHTLParser::getCurrentColumn() const {
    return m_lexer->getCurrentColumn();
}

Token CHTLParser::nextToken() {
    if (!m_hasCurrentToken) {
        m_currentToken = m_lexer->nextToken();
        m_hasCurrentToken = true;
    }
    return m_currentToken;
}

Token CHTLParser::peekToken() {
    return m_lexer->peekToken();
}

void CHTLParser::consumeToken() {
    m_hasCurrentToken = false;
}

bool CHTLParser::match(TokenType type) {
    return nextToken().type == type;
}

bool CHTLParser::match(const std::string& value) {
    return nextToken().value == value;
}

void CHTLParser::expect(TokenType type) {
    Token token = nextToken();
    if (token.type != type) {
        addError("Expected token type " + std::to_string(static_cast<int>(type)) + 
                " but got " + std::to_string(static_cast<int>(token.type)));
    }
}

void CHTLParser::expect(const std::string& value) {
    Token token = nextToken();
    if (token.value != value) {
        addError("Expected '" + value + "' but got '" + token.value + "'");
    }
}

std::string CHTLParser::parseString() {
    // TODO: 实现字符串解析
    return "";
}

std::string CHTLParser::parseLiteral() {
    // TODO: 实现字面量解析
    return "";
}

std::string CHTLParser::parseIdentifier() {
    // TODO: 实现标识符解析
    return "";
}

std::string CHTLParser::parseNumber() {
    // TODO: 实现数字解析
    return "";
}

std::string CHTLParser::parseExpression() {
    // TODO: 实现表达式解析
    return "";
}

std::string CHTLParser::parseConditionalExpression() {
    // TODO: 实现条件表达式解析
    return "";
}

std::string CHTLParser::parseLogicalExpression() {
    // TODO: 实现逻辑表达式解析
    return "";
}

std::string CHTLParser::parseComparisonExpression() {
    // TODO: 实现比较表达式解析
    return "";
}

std::string CHTLParser::parseArithmeticExpression() {
    // TODO: 实现算术表达式解析
    return "";
}

std::string CHTLParser::parsePrimaryExpression() {
    // TODO: 实现主表达式解析
    return "";
}

std::string CHTLParser::parseSelector() {
    // TODO: 实现选择器解析
    return "";
}

std::string CHTLParser::parseClassSelector() {
    // TODO: 实现类选择器解析
    return "";
}

std::string CHTLParser::parseIDSelector() {
    // TODO: 实现ID选择器解析
    return "";
}

std::string CHTLParser::parsePseudoSelector() {
    // TODO: 实现伪选择器解析
    return "";
}

std::string CHTLParser::parseAttributeSelector() {
    // TODO: 实现属性选择器解析
    return "";
}

void CHTLParser::parseTemplateInheritance(std::shared_ptr<BaseNode> node) {
    // TODO: 实现模板继承解析
}

void CHTLParser::parseTemplateVariables(std::shared_ptr<BaseNode> node) {
    // TODO: 实现模板变量解析
}

void CHTLParser::parseTemplateStyles(std::shared_ptr<BaseNode> node) {
    // TODO: 实现模板样式解析
}

void CHTLParser::parseCustomSpecializations(std::shared_ptr<BaseNode> node) {
    // TODO: 实现自定义特例化解析
}

void CHTLParser::parseCustomOperations(std::shared_ptr<BaseNode> node) {
    // TODO: 实现自定义操作解析
}

void CHTLParser::parseImportTarget(std::shared_ptr<BaseNode> node) {
    // TODO: 实现导入目标解析
}

void CHTLParser::parseImportOptions(std::shared_ptr<BaseNode> node) {
    // TODO: 实现导入选项解析
}

void CHTLParser::parseConfigValues(std::shared_ptr<BaseNode> node) {
    // TODO: 实现配置值解析
}

void CHTLParser::parseNameConfigs(std::shared_ptr<BaseNode> node) {
    // TODO: 实现名称配置解析
}

void CHTLParser::parseGroupOptions(std::shared_ptr<BaseNode> node) {
    // TODO: 实现组选项解析
}

void CHTLParser::parseNamespaceContent(std::shared_ptr<BaseNode> node) {
    // TODO: 实现命名空间内容解析
}

void CHTLParser::parseNamespaceConstraints(std::shared_ptr<BaseNode> node) {
    // TODO: 实现命名空间约束解析
}

void CHTLParser::parseDeleteOperation(std::shared_ptr<BaseNode> node) {
    // TODO: 实现删除操作解析
}

void CHTLParser::parseInsertOperation(std::shared_ptr<BaseNode> node) {
    // TODO: 实现插入操作解析
}

void CHTLParser::parseInheritOperation(std::shared_ptr<BaseNode> node) {
    // TODO: 实现继承操作解析
}

void CHTLParser::parseUseOperation(std::shared_ptr<BaseNode> node) {
    // TODO: 实现使用操作解析
}

bool CHTLParser::isHTMLElement(const std::string& tag) {
    // 常见的HTML元素列表
    static const std::unordered_set<std::string> htmlElements = {
        "html", "head", "title", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "button", "textarea", "select", "option", "label", "fieldset", "legend",
        "nav", "header", "footer", "main", "section", "article", "aside", "figure", "figcaption",
        "blockquote", "code", "pre", "em", "strong", "b", "i", "u", "s", "mark", "small", "sub", "sup",
        "br", "hr", "meta", "link", "script", "style", "noscript", "iframe", "canvas", "svg",
        "audio", "video", "source", "track", "embed", "object", "param", "map", "area",
        "details", "summary", "dialog", "menu", "menuitem", "progress", "meter", "output",
        "datalist", "keygen", "command", "bdi", "bdo", "ruby", "rt", "rp", "wbr"
    };
    
    return htmlElements.find(tag) != htmlElements.end();
}

} // namespace CHTL