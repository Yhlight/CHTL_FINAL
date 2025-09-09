#include "CHTL/CHTLParser.h"
#include <sstream>

namespace CHTL {

// CHTLParser 实现
CHTLParser::CHTLParser(std::istream& input)
    : lexer_(input) {
    initializeDefaultConfig();
}

void CHTLParser::initializeDefaultConfig() {
    // 初始化默认配置
    config_.debugMode = false;
    config_.indexInitialCount = 0;
    config_.disableStyleAutoAddClass = false;
    config_.disableStyleAutoAddId = false;
    config_.disableScriptAutoAddClass = true;
    config_.disableScriptAutoAddId = true;
    config_.disableDefaultNamespace = false;
    config_.disableCustomOriginType = false;
    config_.disableNameGroup = false;
}

std::shared_ptr<CHTLNode> CHTLParser::parse() {
    rootNode_ = std::make_shared<ElementNode>("root");
    
    while (!lexer_.isEof()) {
        auto node = parseNode();
        if (node) {
            rootNode_->addChild(node);
        }
    }
    
    return rootNode_;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNode() {
    Token token = lexer_.peekToken();
    
    switch (token.type) {
        case TokenType::Identifier:
            return parseElement();
        case TokenType::LeftBracket:
            return parseBlock();
        case TokenType::String:
            return parseText();
        case TokenType::Comment:
        case TokenType::GeneratorComment:
            return parseComment();
        default:
            addError("Unexpected token: " + token.value, token.position);
            lexer_.skipToken();
            return nullptr;
    }
}

std::shared_ptr<ElementNode> CHTLParser::parseElement() {
    Token token = lexer_.consume(TokenType::Identifier);
    auto element = std::make_shared<ElementNode>(token.value, token.position);
    
    // 解析属性
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                element->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return element;
}

std::shared_ptr<TextNode> CHTLParser::parseText() {
    Token token = lexer_.consume(TokenType::String);
    return std::make_shared<TextNode>(token.value, token.position);
}

std::shared_ptr<StyleNode> CHTLParser::parseStyle() {
    Token token = lexer_.consume(TokenType::Keyword);
    auto style = std::make_shared<StyleNode>(token.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        String content;
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            Token token = lexer_.nextToken();
            content += token.value + " ";
        }
        
        style->setStyleContent(content);
        lexer_.consume(TokenType::RightBrace);
    }
    
    return style;
}

std::shared_ptr<ScriptNode> CHTLParser::parseScript() {
    Token token = lexer_.consume(TokenType::Keyword);
    auto script = std::make_shared<ScriptNode>(token.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        String content;
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            Token token = lexer_.nextToken();
            content += token.value + " ";
        }
        
        script->setScriptContent(content);
        lexer_.consume(TokenType::RightBrace);
    }
    
    return script;
}

std::shared_ptr<TemplateNode> CHTLParser::parseTemplate() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("[Template]")) {
        lexer_.consume(TokenType::Keyword);
        Token typeToken = lexer_.consume(TokenType::Keyword);
        Token nameToken = lexer_.consume(TokenType::Identifier);
        
        if (typeToken.value == "@Style") {
            return parseTemplateStyle();
        } else if (typeToken.value == "@Element") {
            return parseTemplateElement();
        } else if (typeToken.value == "@Var") {
            return parseTemplateVar();
        }
    }
    
    addError("Invalid template syntax", token.position);
    return nullptr;
}

std::shared_ptr<CustomNode> CHTLParser::parseCustom() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("[Custom]")) {
        lexer_.consume(TokenType::Keyword);
        Token typeToken = lexer_.consume(TokenType::Keyword);
        Token nameToken = lexer_.consume(TokenType::Identifier);
        
        if (typeToken.value == "@Style") {
            return parseCustomStyle();
        } else if (typeToken.value == "@Element") {
            return parseCustomElement();
        } else if (typeToken.value == "@Var") {
            return parseCustomVar();
        }
    }
    
    addError("Invalid custom syntax", token.position);
    return nullptr;
}

std::shared_ptr<OriginNode> CHTLParser::parseOrigin() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("[Origin]")) {
        lexer_.consume(TokenType::Keyword);
        Token typeToken = lexer_.consume(TokenType::Keyword);
        Token nameToken = lexer_.peekToken();
        
        String name;
        if (nameToken.type == TokenType::Identifier) {
            name = lexer_.consume(TokenType::Identifier).value;
        }
        
        if (typeToken.value == "@Html") {
            return parseOriginHtml();
        } else if (typeToken.value == "@Style") {
            return parseOriginStyle();
        } else if (typeToken.value == "@JavaScript") {
            return parseOriginJavascript();
        }
    }
    
    addError("Invalid origin syntax", token.position);
    return nullptr;
}

std::shared_ptr<ImportNode> CHTLParser::parseImport() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("[Import]")) {
        lexer_.consume(TokenType::Keyword);
        Token typeToken = lexer_.consume(TokenType::Keyword);
        Token pathToken = lexer_.consume(TokenType::String);
        Token aliasToken = lexer_.peekToken();
        
        String alias;
        if (aliasToken.isKeyword("as")) {
            lexer_.consume(TokenType::Keyword);
            alias = lexer_.consume(TokenType::Identifier).value;
        }
        
        if (typeToken.value == "@Html") {
            return parseImportHtml();
        } else if (typeToken.value == "@Style") {
            return parseImportStyle();
        } else if (typeToken.value == "@JavaScript") {
            return parseImportJavascript();
        } else if (typeToken.value == "@Chtl") {
            return parseImportChtl();
        } else if (typeToken.value == "@CJmod") {
            return parseImportCjmod();
        }
    }
    
    addError("Invalid import syntax", token.position);
    return nullptr;
}

std::shared_ptr<ConfigNode> CHTLParser::parseConfig() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("[Configuration]")) {
        lexer_.consume(TokenType::Keyword);
        Token nameToken = lexer_.peekToken();
        
        String name;
        if (nameToken.type == TokenType::Identifier) {
            name = lexer_.consume(TokenType::Identifier).value;
        }
        
        return parseConfiguration();
    }
    
    addError("Invalid configuration syntax", token.position);
    return nullptr;
}

std::shared_ptr<NamespaceNode> CHTLParser::parseNamespace() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("[Namespace]")) {
        lexer_.consume(TokenType::Keyword);
        Token nameToken = lexer_.consume(TokenType::Identifier);
        
        return std::make_shared<NamespaceNode>(nameToken.value, nameToken.position);
    }
    
    addError("Invalid namespace syntax", token.position);
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseBlock() {
    lexer_.consume(TokenType::LeftBracket);
    
    auto block = std::make_shared<ElementNode>("block");
    
    while (lexer_.peekToken().type != TokenType::RightBracket && !lexer_.isEof()) {
        auto child = parseNode();
        if (child) {
            block->addChild(child);
        }
    }
    
    lexer_.consume(TokenType::RightBracket);
    return block;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStatement() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("text")) {
        return parseText();
    } else if (token.isKeyword("style")) {
        return parseStyle();
    } else if (token.isKeyword("script")) {
        return parseScript();
    } else if (token.isKeyword("[Template]")) {
        return parseTemplate();
    } else if (token.isKeyword("[Custom]")) {
        return parseCustom();
    } else if (token.isKeyword("[Origin]")) {
        return parseOrigin();
    } else if (token.isKeyword("[Import]")) {
        return parseImport();
    } else if (token.isKeyword("[Configuration]")) {
        return parseConfig();
    } else if (token.isKeyword("[Namespace]")) {
        return parseNamespace();
    } else {
        return parseElement();
    }
}

AttributeList CHTLParser::parseAttributes() {
    AttributeList attributes;
    
    while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
        auto attr = parseAttribute();
        if (attr.name.empty()) {
            break;
        }
        attributes.push_back(attr);
    }
    
    return attributes;
}

Attribute CHTLParser::parseAttribute() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    Token colonToken = lexer_.consume(TokenType::Colon);
    Token valueToken = lexer_.consume(TokenType::String);
    Token semicolonToken = lexer_.consume(TokenType::Semicolon);
    
    return Attribute(nameToken.value, valueToken.value, true);
}

std::shared_ptr<TemplateStyleNode> CHTLParser::parseTemplateStyle() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    auto template_ = std::make_shared<TemplateStyleNode>(nameToken.value, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                template_->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return template_;
}

std::shared_ptr<TemplateElementNode> CHTLParser::parseTemplateElement() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    auto template_ = std::make_shared<TemplateElementNode>(nameToken.value, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                template_->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return template_;
}

std::shared_ptr<TemplateVarNode> CHTLParser::parseTemplateVar() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    auto template_ = std::make_shared<TemplateVarNode>(nameToken.value, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                template_->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return template_;
}

std::shared_ptr<CustomStyleNode> CHTLParser::parseCustomStyle() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    auto custom = std::make_shared<CustomStyleNode>(nameToken.value, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                custom->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return custom;
}

std::shared_ptr<CustomElementNode> CHTLParser::parseCustomElement() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    auto custom = std::make_shared<CustomElementNode>(nameToken.value, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                custom->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return custom;
}

std::shared_ptr<CustomVarNode> CHTLParser::parseCustomVar() {
    Token nameToken = lexer_.consume(TokenType::Identifier);
    auto custom = std::make_shared<CustomVarNode>(nameToken.value, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            auto child = parseNode();
            if (child) {
                custom->addChild(child);
            }
        }
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return custom;
}

std::shared_ptr<OriginNode> CHTLParser::parseOriginHtml() {
    Token nameToken = lexer_.peekToken();
    String name;
    if (nameToken.type == TokenType::Identifier) {
        name = lexer_.consume(TokenType::Identifier).value;
    }
    
    auto origin = std::make_shared<OriginNode>("@Html", name, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        String content;
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            Token token = lexer_.nextToken();
            content += token.value + " ";
        }
        
        origin->setOriginContent(content);
        lexer_.consume(TokenType::RightBrace);
    }
    
    return origin;
}

std::shared_ptr<OriginNode> CHTLParser::parseOriginStyle() {
    Token nameToken = lexer_.peekToken();
    String name;
    if (nameToken.type == TokenType::Identifier) {
        name = lexer_.consume(TokenType::Identifier).value;
    }
    
    auto origin = std::make_shared<OriginNode>("@Style", name, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        String content;
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            Token token = lexer_.nextToken();
            content += token.value + " ";
        }
        
        origin->setOriginContent(content);
        lexer_.consume(TokenType::RightBrace);
    }
    
    return origin;
}

std::shared_ptr<OriginNode> CHTLParser::parseOriginJavascript() {
    Token nameToken = lexer_.peekToken();
    String name;
    if (nameToken.type == TokenType::Identifier) {
        name = lexer_.consume(TokenType::Identifier).value;
    }
    
    auto origin = std::make_shared<OriginNode>("@JavaScript", name, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        String content;
        while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
            Token token = lexer_.nextToken();
            content += token.value + " ";
        }
        
        origin->setOriginContent(content);
        lexer_.consume(TokenType::RightBrace);
    }
    
    return origin;
}

std::shared_ptr<ImportNode> CHTLParser::parseImportHtml() {
    Token pathToken = lexer_.consume(TokenType::String);
    Token aliasToken = lexer_.peekToken();
    
    String alias;
    if (aliasToken.isKeyword("as")) {
        lexer_.consume(TokenType::Keyword);
        alias = lexer_.consume(TokenType::Identifier).value;
    }
    
    return std::make_shared<ImportNode>("@Html", pathToken.value, alias, pathToken.position);
}

std::shared_ptr<ImportNode> CHTLParser::parseImportStyle() {
    Token pathToken = lexer_.consume(TokenType::String);
    Token aliasToken = lexer_.peekToken();
    
    String alias;
    if (aliasToken.isKeyword("as")) {
        lexer_.consume(TokenType::Keyword);
        alias = lexer_.consume(TokenType::Identifier).value;
    }
    
    return std::make_shared<ImportNode>("@Style", pathToken.value, alias, pathToken.position);
}

std::shared_ptr<ImportNode> CHTLParser::parseImportJavascript() {
    Token pathToken = lexer_.consume(TokenType::String);
    Token aliasToken = lexer_.peekToken();
    
    String alias;
    if (aliasToken.isKeyword("as")) {
        lexer_.consume(TokenType::Keyword);
        alias = lexer_.consume(TokenType::Identifier).value;
    }
    
    return std::make_shared<ImportNode>("@JavaScript", pathToken.value, alias, pathToken.position);
}

std::shared_ptr<ImportNode> CHTLParser::parseImportChtl() {
    Token pathToken = lexer_.consume(TokenType::String);
    Token aliasToken = lexer_.peekToken();
    
    String alias;
    if (aliasToken.isKeyword("as")) {
        lexer_.consume(TokenType::Keyword);
        alias = lexer_.consume(TokenType::Identifier).value;
    }
    
    return std::make_shared<ImportNode>("@Chtl", pathToken.value, alias, pathToken.position);
}

std::shared_ptr<ImportNode> CHTLParser::parseImportCjmod() {
    Token pathToken = lexer_.consume(TokenType::String);
    Token aliasToken = lexer_.peekToken();
    
    String alias;
    if (aliasToken.isKeyword("as")) {
        lexer_.consume(TokenType::Keyword);
        alias = lexer_.consume(TokenType::Identifier).value;
    }
    
    return std::make_shared<ImportNode>("@CJmod", pathToken.value, alias, pathToken.position);
}

std::shared_ptr<ConfigNode> CHTLParser::parseConfiguration() {
    Token nameToken = lexer_.peekToken();
    String name;
    if (nameToken.type == TokenType::Identifier) {
        name = lexer_.consume(TokenType::Identifier).value;
    }
    
    auto config = std::make_shared<ConfigNode>(name, nameToken.position);
    
    if (lexer_.peekToken().type == TokenType::LeftBrace) {
        lexer_.consume(TokenType::LeftBrace);
        
        ConfigOptions options;
        parseConfigOptions(options);
        config->setConfigOptions(options);
        
        lexer_.consume(TokenType::RightBrace);
    }
    
    return config;
}

void CHTLParser::parseConfigOptions(ConfigOptions& options) {
    while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
        Token token = lexer_.peekToken();
        
        if (token.type == TokenType::Identifier) {
            String optionName = lexer_.consume(TokenType::Identifier).value;
            lexer_.consume(TokenType::Equal);
            Token valueToken = lexer_.consume(TokenType::String);
            lexer_.consume(TokenType::Semicolon);
            
            // 设置配置选项
            if (optionName == "DEBUG_MODE") {
                options.debugMode = (valueToken.value == "true");
            } else if (optionName == "INDEX_INITIAL_COUNT") {
                options.indexInitialCount = std::stoi(valueToken.value);
            } else if (optionName == "DISABLE_STYLE_AUTO_ADD_CLASS") {
                options.disableStyleAutoAddClass = (valueToken.value == "true");
            } else if (optionName == "DISABLE_STYLE_AUTO_ADD_ID") {
                options.disableStyleAutoAddId = (valueToken.value == "true");
            } else if (optionName == "DISABLE_SCRIPT_AUTO_ADD_CLASS") {
                options.disableScriptAutoAddClass = (valueToken.value == "true");
            } else if (optionName == "DISABLE_SCRIPT_AUTO_ADD_ID") {
                options.disableScriptAutoAddId = (valueToken.value == "true");
            } else if (optionName == "DISABLE_DEFAULT_NAMESPACE") {
                options.disableDefaultNamespace = (valueToken.value == "true");
            } else if (optionName == "DISABLE_CUSTOM_ORIGIN_TYPE") {
                options.disableCustomOriginType = (valueToken.value == "true");
            } else if (optionName == "DISABLE_NAME_GROUP") {
                options.disableNameGroup = (valueToken.value == "true");
            } else if (optionName == "OPTION_COUNT") {
                options.optionCount = std::stoi(valueToken.value);
            }
        } else if (token.isKeyword("[Name]")) {
            lexer_.consume(TokenType::Keyword);
            parseNameGroup(options);
        } else {
            lexer_.skipToken();
        }
    }
}

void CHTLParser::parseNameGroup(ConfigOptions& options) {
    lexer_.consume(TokenType::LeftBrace);
    
    while (lexer_.peekToken().type != TokenType::RightBrace && !lexer_.isEof()) {
        Token token = lexer_.peekToken();
        
        if (token.type == TokenType::Identifier) {
            String optionName = lexer_.consume(TokenType::Identifier).value;
            lexer_.consume(TokenType::Equal);
            
            if (optionName == "CUSTOM_STYLE") {
                // 解析数组
                lexer_.consume(TokenType::LeftBracket);
                StringList values;
                while (lexer_.peekToken().type != TokenType::RightBracket && !lexer_.isEof()) {
                    Token valueToken = lexer_.consume(TokenType::String);
                    values.push_back(valueToken.value);
                    if (lexer_.peekToken().type == TokenType::Comma) {
                        lexer_.consume(TokenType::Comma);
                    }
                }
                lexer_.consume(TokenType::RightBracket);
                options.customStyleKeywords = values;
            } else {
                Token valueToken = lexer_.consume(TokenType::String);
                lexer_.consume(TokenType::Semicolon);
                
                // 设置其他关键字
                if (optionName == "CUSTOM_ELEMENT") {
                    options.customElementKeyword = valueToken.value;
                } else if (optionName == "CUSTOM_VAR") {
                    options.customVarKeyword = valueToken.value;
                } else if (optionName == "TEMPLATE_STYLE") {
                    options.templateStyleKeyword = valueToken.value;
                } else if (optionName == "TEMPLATE_ELEMENT") {
                    options.templateElementKeyword = valueToken.value;
                } else if (optionName == "TEMPLATE_VAR") {
                    options.templateVarKeyword = valueToken.value;
                } else if (optionName == "ORIGIN_HTML") {
                    options.originHtmlKeyword = valueToken.value;
                } else if (optionName == "ORIGIN_STYLE") {
                    options.originStyleKeyword = valueToken.value;
                } else if (optionName == "ORIGIN_JAVASCRIPT") {
                    options.originJavascriptKeyword = valueToken.value;
                } else if (optionName == "IMPORT_HTML") {
                    options.importHtmlKeyword = valueToken.value;
                } else if (optionName == "IMPORT_STYLE") {
                    options.importStyleKeyword = valueToken.value;
                } else if (optionName == "IMPORT_JAVASCRIPT") {
                    options.importJavascriptKeyword = valueToken.value;
                } else if (optionName == "IMPORT_CHTL") {
                    options.importChtlKeyword = valueToken.value;
                } else if (optionName == "IMPORT_CJMOD") {
                    options.importCjmodKeyword = valueToken.value;
                } else if (optionName == "KEYWORD_INHERIT") {
                    options.keywordInherit = valueToken.value;
                } else if (optionName == "KEYWORD_DELETE") {
                    options.keywordDelete = valueToken.value;
                } else if (optionName == "KEYWORD_INSERT") {
                    options.keywordInsert = valueToken.value;
                } else if (optionName == "KEYWORD_AFTER") {
                    options.keywordAfter = valueToken.value;
                } else if (optionName == "KEYWORD_BEFORE") {
                    options.keywordBefore = valueToken.value;
                } else if (optionName == "KEYWORD_REPLACE") {
                    options.keywordReplace = valueToken.value;
                } else if (optionName == "KEYWORD_ATTOP") {
                    options.keywordAtTop = valueToken.value;
                } else if (optionName == "KEYWORD_ATBOTTOM") {
                    options.keywordAtBottom = valueToken.value;
                } else if (optionName == "KEYWORD_FROM") {
                    options.keywordFrom = valueToken.value;
                } else if (optionName == "KEYWORD_AS") {
                    options.keywordAs = valueToken.value;
                } else if (optionName == "KEYWORD_EXCEPT") {
                    options.keywordExcept = valueToken.value;
                } else if (optionName == "KEYWORD_USE") {
                    options.keywordUse = valueToken.value;
                } else if (optionName == "KEYWORD_HTML5") {
                    options.keywordHtml5 = valueToken.value;
                } else if (optionName == "KEYWORD_TEXT") {
                    options.keywordText = valueToken.value;
                } else if (optionName == "KEYWORD_STYLE") {
                    options.keywordStyle = valueToken.value;
                } else if (optionName == "KEYWORD_SCRIPT") {
                    options.keywordScript = valueToken.value;
                } else if (optionName == "KEYWORD_CUSTOM") {
                    options.keywordCustom = valueToken.value;
                } else if (optionName == "KEYWORD_TEMPLATE") {
                    options.keywordTemplate = valueToken.value;
                } else if (optionName == "KEYWORD_ORIGIN") {
                    options.keywordOrigin = valueToken.value;
                } else if (optionName == "KEYWORD_IMPORT") {
                    options.keywordImport = valueToken.value;
                } else if (optionName == "KEYWORD_NAMESPACE") {
                    options.keywordNamespace = valueToken.value;
                } else if (optionName == "KEYWORD_CONFIGURATION") {
                    options.keywordConfiguration = valueToken.value;
                }
            }
        } else {
            lexer_.skipToken();
        }
    }
    
    lexer_.consume(TokenType::RightBrace);
}

std::shared_ptr<OperatorNode> CHTLParser::parseOperator() {
    Token token = lexer_.peekToken();
    
    if (token.isKeyword("delete")) {
        lexer_.consume(TokenType::Keyword);
        return std::make_shared<OperatorNode>(OperatorNode::OperatorType::Delete, token.position);
    } else if (token.isKeyword("insert")) {
        lexer_.consume(TokenType::Keyword);
        return std::make_shared<OperatorNode>(OperatorNode::OperatorType::Insert, token.position);
    } else if (token.isKeyword("use")) {
        lexer_.consume(TokenType::Keyword);
        return std::make_shared<OperatorNode>(OperatorNode::OperatorType::Use, token.position);
    } else if (token.isKeyword("inherit")) {
        lexer_.consume(TokenType::Keyword);
        return std::make_shared<OperatorNode>(OperatorNode::OperatorType::Inherit, token.position);
    }
    
    addError("Invalid operator: " + token.value, token.position);
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseExpression() {
    return parseConditionalExpression();
}

std::shared_ptr<CHTLNode> CHTLParser::parseConditionalExpression() {
    auto left = parseLogicalExpression();
    
    if (lexer_.peekToken().isOperator("?")) {
        lexer_.consume(TokenType::Keyword);
        auto trueExpr = parseExpression();
        lexer_.consume(TokenType::Colon);
        auto falseExpr = parseExpression();
        
        // 创建条件表达式节点
        auto condition = std::make_shared<ElementNode>("condition");
        condition->addChild(left);
        condition->addChild(trueExpr);
        condition->addChild(falseExpr);
        return condition;
    }
    
    return left;
}

std::shared_ptr<CHTLNode> CHTLParser::parseLogicalExpression() {
    auto left = parseComparisonExpression();
    
    while (lexer_.peekToken().isOperator("&&") || lexer_.peekToken().isOperator("||")) {
        Token op = lexer_.consume(TokenType::Keyword);
        auto right = parseComparisonExpression();
        
        // 创建逻辑表达式节点
        auto logical = std::make_shared<ElementNode>("logical");
        logical->setAttribute("operator", op.value);
        logical->addChild(left);
        logical->addChild(right);
        left = logical;
    }
    
    return left;
}

std::shared_ptr<CHTLNode> CHTLParser::parseComparisonExpression() {
    auto left = parseArithmeticExpression();
    
    while (lexer_.peekToken().isOperator("==") || lexer_.peekToken().isOperator("!=") ||
           lexer_.peekToken().isOperator("<") || lexer_.peekToken().isOperator(">") ||
           lexer_.peekToken().isOperator("<=") || lexer_.peekToken().isOperator(">=")) {
        Token op = lexer_.consume(TokenType::Keyword);
        auto right = parseArithmeticExpression();
        
        // 创建比较表达式节点
        auto comparison = std::make_shared<ElementNode>("comparison");
        comparison->setAttribute("operator", op.value);
        comparison->addChild(left);
        comparison->addChild(right);
        left = comparison;
    }
    
    return left;
}

std::shared_ptr<CHTLNode> CHTLParser::parseArithmeticExpression() {
    auto left = parsePrimaryExpression();
    
    while (lexer_.peekToken().isOperator("+") || lexer_.peekToken().isOperator("-") ||
           lexer_.peekToken().isOperator("*") || lexer_.peekToken().isOperator("/") ||
           lexer_.peekToken().isOperator("%")) {
        Token op = lexer_.consume(TokenType::Keyword);
        auto right = parsePrimaryExpression();
        
        // 创建算术表达式节点
        auto arithmetic = std::make_shared<ElementNode>("arithmetic");
        arithmetic->setAttribute("operator", op.value);
        arithmetic->addChild(left);
        arithmetic->addChild(right);
        left = arithmetic;
    }
    
    return left;
}

std::shared_ptr<CHTLNode> CHTLParser::parsePrimaryExpression() {
    Token token = lexer_.peekToken();
    
    if (token.type == TokenType::String || token.type == TokenType::Number) {
        lexer_.consume(token.type);
        return std::make_shared<TextNode>(token.value, token.position);
    } else if (token.type == TokenType::Identifier) {
        lexer_.consume(TokenType::Identifier);
        return std::make_shared<ElementNode>(token.value, token.position);
    } else if (token.type == TokenType::LeftParen) {
        lexer_.consume(TokenType::LeftParen);
        auto expr = parseExpression();
        lexer_.consume(TokenType::RightParen);
        return expr;
    }
    
    addError("Invalid primary expression", token.position);
    return nullptr;
}

bool CHTLParser::match(TokenType type) {
    return lexer_.peekToken().type == type;
}

bool CHTLParser::match(const String& keyword) {
    return lexer_.peekToken().isKeyword(keyword);
}

bool CHTLParser::matchOperator(const String& op) {
    return lexer_.peekToken().isOperator(op);
}

Token CHTLParser::consume(TokenType type) {
    Token token = lexer_.nextToken();
    if (token.type != type) {
        addError("Expected " + std::to_string(static_cast<int>(type)) + ", got " + std::to_string(static_cast<int>(token.type)), token.position);
    }
    return token;
}

Token CHTLParser::consume(const String& keyword) {
    Token token = lexer_.nextToken();
    if (!token.isKeyword(keyword)) {
        addError("Expected keyword " + keyword + ", got " + token.value, token.position);
    }
    return token;
}

Token CHTLParser::consumeOperator(const String& op) {
    Token token = lexer_.nextToken();
    if (!token.isOperator(op)) {
        addError("Expected operator " + op + ", got " + token.value, token.position);
    }
    return token;
}

Token CHTLParser::expect(TokenType type) {
    return consume(type);
}

Token CHTLParser::expect(const String& keyword) {
    return consume(keyword);
}

Token CHTLParser::expectOperator(const String& op) {
    return consumeOperator(op);
}

void CHTLParser::addError(const String& message, const Position& pos) {
    errors_.emplace_back(message, pos, "error");
}

void CHTLParser::addError(const String& message, const Token& token) {
    addError(message, token.position);
}

void CHTLParser::synchronize() {
    while (!lexer_.isEof()) {
        Token token = lexer_.peekToken();
        if (token.type == TokenType::Semicolon) {
            lexer_.skipToken();
            break;
        }
        if (token.type == TokenType::RightBrace) {
            break;
        }
        lexer_.skipToken();
    }
}

void CHTLParser::skipToBlockEnd() {
    while (!lexer_.isEof()) {
        Token token = lexer_.peekToken();
        if (token.type == TokenType::RightBrace) {
            break;
        }
        lexer_.skipToken();
    }
}

void CHTLParser::skipToStatementEnd() {
    while (!lexer_.isEof()) {
        Token token = lexer_.peekToken();
        if (token.type == TokenType::Semicolon || token.type == TokenType::RightBrace) {
            break;
        }
        lexer_.skipToken();
    }
}

Token CHTLParser::peek() {
    return lexer_.peekToken();
}

Token CHTLParser::peekNext() {
    return lexer_.peekToken();
}

bool CHTLParser::isAtEnd() {
    return lexer_.isEof();
}

bool CHTLParser::isValidElementName(const String& name) const {
    // 检查是否为有效的 HTML 元素名
    static const StringSet validElements = {
        "html", "head", "body", "div", "span", "p", "a", "img", "ul", "ol", "li",
        "table", "tr", "td", "th", "form", "input", "button", "select", "option",
        "textarea", "label", "h1", "h2", "h3", "h4", "h5", "h6", "br", "hr",
        "meta", "title", "link", "script", "style"
    };
    
    return validElements.find(name) != validElements.end();
}

bool CHTLParser::isValidAttributeName(const String& name) const {
    // 检查是否为有效的 HTML 属性名
    static const StringSet validAttributes = {
        "id", "class", "style", "title", "alt", "src", "href", "type", "value",
        "name", "placeholder", "required", "disabled", "checked", "selected",
        "width", "height", "colspan", "rowspan", "target", "rel", "media"
    };
    
    return validAttributes.find(name) != validAttributes.end();
}

bool CHTLParser::isValidTemplateName(const String& name) const {
    // 检查是否为有效的模板名
    return !name.empty() && name[0] != '_' && name.find(' ') == String::npos;
}

bool CHTLParser::isValidCustomName(const String& name) const {
    // 检查是否为有效的自定义名
    return !name.empty() && name[0] != '_' && name.find(' ') == String::npos;
}

String CHTLParser::unescapeString(const String& str) const {
    String result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += str[i + 1]; break;
            }
            ++i; // 跳过下一个字符
        } else {
            result += str[i];
        }
    }
    return result;
}

String CHTLParser::escapeString(const String& str) const {
    String result;
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default: result += c; break;
        }
    }
    return result;
}

double CHTLParser::parseNumber(const String& str) const {
    try {
        return std::stod(str);
    } catch (const std::exception&) {
        return 0.0;
    }
}

bool CHTLParser::isValidNumber(const String& str) const {
    try {
        std::stod(str);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

const ErrorList& CHTLParser::getErrors() const {
    return errors_;
}

void CHTLParser::setConfig(const ConfigOptions& config) {
    config_ = config;
}

} // namespace CHTL