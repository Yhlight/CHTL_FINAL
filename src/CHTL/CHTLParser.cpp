#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(CHTLContext& context)
    : context_(context), currentIndex_(0), lexer_("") {
}

CHTLParser::~CHTLParser() = default;

std::shared_ptr<BaseNode> CHTLParser::parse(const std::string& source) {
    lexer_ = Lexer(source);
    tokens_ = lexer_.tokenize();
    currentIndex_ = 0;
    
    return parseProgram();
}

std::shared_ptr<BaseNode> CHTLParser::parse(const TokenList& tokens) {
    tokens_ = tokens;
    currentIndex_ = 0;
    
    return parseProgram();
}

std::shared_ptr<BaseNode> CHTLParser::parseProgram() {
    // 创建一个根节点来包含所有顶级节点
    auto root = std::make_shared<ElementNode>("root");
    
    while (!isCurrentToken(TokenType::END_OF_FILE)) {
        skipWhitespace();
        if (isCurrentToken(TokenType::END_OF_FILE)) break;
        
        std::shared_ptr<BaseNode> node = nullptr;
        
        if (isCurrentToken(TokenType::TEMPLATE)) {
            node = parseTemplate();
        } else if (isCurrentToken(TokenType::CUSTOM)) {
            node = parseCustom();
        } else if (isCurrentToken(TokenType::ORIGIN)) {
            node = parseOrigin();
        } else if (isCurrentToken(TokenType::IMPORT)) {
            node = parseImport();
        } else if (isCurrentToken(TokenType::CONFIGURATION)) {
            node = parseConfiguration();
        } else if (isCurrentToken(TokenType::NAMESPACE)) {
            node = parseNamespace();
        } else if (isCurrentToken(TokenType::GENERATOR_COMMENT)) {
            node = parseComment();
        } else if (isCurrentToken(TokenType::IDENTIFIER)) {
            node = parseElement();
        } else {
            reportError("Unexpected token: " + currentToken().toString());
            nextToken(); // 跳过错误的 token
        }
        
        if (node) {
            root->addChild(node);
        }
    }
    
    return root;
}

std::shared_ptr<ElementNode> CHTLParser::parseElement() {
    if (!isCurrentToken(TokenType::IDENTIFIER)) {
        reportError("Expected element name");
        return nullptr;
    }
    
    std::string tagName = currentToken().value;
    size_t line = currentToken().line;
    size_t column = currentToken().column;
    
    auto element = std::make_shared<ElementNode>(tagName, line, column);
    nextToken(); // 跳过元素名
    
    // 解析属性
    parseAttributes(element);
    
    // 检查是否有子内容
    if (isCurrentToken(TokenType::LBRACE)) {
        nextToken(); // 跳过 {
        
        while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
            skipWhitespace();
            if (isCurrentToken(TokenType::RBRACE)) break;
            
            std::shared_ptr<BaseNode> child = nullptr;
            
            if (isCurrentToken(TokenType::TEXT)) {
                child = parseText();
            } else if (isCurrentToken(TokenType::IDENTIFIER)) {
                child = parseElement();
            } else if (isCurrentToken(TokenType::STYLE)) {
                child = parseStyleBlock();
            } else if (isCurrentToken(TokenType::GENERATOR_COMMENT)) {
                child = parseComment();
            } else {
                reportError("Unexpected token in element: " + currentToken().toString());
                nextToken();
            }
            
            if (child) {
                element->addChild(child);
            }
        }
        
        if (isCurrentToken(TokenType::RBRACE)) {
            nextToken(); // 跳过 }
        } else {
            reportError("Expected closing brace for element");
        }
    }
    
    return element;
}

std::shared_ptr<TextNode> CHTLParser::parseText() {
    if (isCurrentToken(TokenType::TEXT)) {
        std::string text = currentToken().value;
        size_t line = currentToken().line;
        size_t column = currentToken().column;
        nextToken();
        return std::make_shared<TextNode>(text, line, column);
    }
    
    // 处理 text { } 语法
    if (isCurrentToken(TokenType::TEXT) && peekToken().type == TokenType::LBRACE) {
        nextToken(); // 跳过 text
        expectToken(TokenType::LBRACE); // 跳过 {
        
        std::string text;
        while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
            if (isCurrentToken(TokenType::STRING)) {
                text += currentToken().value;
            } else if (isCurrentToken(TokenType::IDENTIFIER)) {
                text += currentToken().value;
            }
            nextToken();
        }
        
        expectToken(TokenType::RBRACE); // 跳过 }
        
        return std::make_shared<TextNode>(text, 0, 0);
    }
    
    return nullptr;
}

void CHTLParser::parseAttributes(std::shared_ptr<ElementNode> element) {
    while (!isCurrentToken(TokenType::LBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        if (isCurrentToken(TokenType::IDENTIFIER)) {
            std::string attrName = currentToken().value;
            nextToken();
            
            std::string attrValue;
            if (isCurrentToken(TokenType::COLON) || isCurrentToken(TokenType::EQUALS)) {
                nextToken(); // 跳过 : 或 =
                
                if (isCurrentToken(TokenType::STRING)) {
                    attrValue = currentToken().value;
                    nextToken();
                } else if (isCurrentToken(TokenType::IDENTIFIER)) {
                    attrValue = currentToken().value;
                    nextToken();
                }
            }
            
            element->setAttribute(attrName, attrValue);
        } else {
            break;
        }
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleBlock() {
    // 简化实现，返回一个空的样式节点
    // 实际实现需要解析 CSS 语法
    expectToken(TokenType::STYLE);
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("style");
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    // 简化实现
    expectToken(TokenType::TEMPLATE);
    nextToken(); // 跳过模板类型
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("template");
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    // 简化实现
    expectToken(TokenType::CUSTOM);
    nextToken(); // 跳过自定义类型
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("custom");
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    // 简化实现
    expectToken(TokenType::ORIGIN);
    nextToken(); // 跳过原始嵌入类型
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("origin");
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    // 简化实现
    expectToken(TokenType::IMPORT);
    nextToken(); // 跳过导入类型
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("import");
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    // 简化实现
    expectToken(TokenType::CONFIGURATION);
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("configuration");
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    // 简化实现
    expectToken(TokenType::NAMESPACE);
    nextToken(); // 跳过命名空间名
    expectToken(TokenType::LBRACE);
    
    while (!isCurrentToken(TokenType::RBRACE) && !isCurrentToken(TokenType::END_OF_FILE)) {
        nextToken();
    }
    
    expectToken(TokenType::RBRACE);
    
    return std::make_shared<ElementNode>("namespace");
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    if (isCurrentToken(TokenType::GENERATOR_COMMENT)) {
        std::string comment = currentToken().value;
        size_t line = currentToken().line;
        size_t column = currentToken().column;
        nextToken();
        return std::make_shared<TextNode>(comment, line, column);
    }
    return nullptr;
}

Token CHTLParser::currentToken() const {
    if (currentIndex_ >= tokens_.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0, 0);
    }
    return tokens_[currentIndex_];
}

Token CHTLParser::nextToken() {
    if (currentIndex_ < tokens_.size()) {
        ++currentIndex_;
    }
    return currentToken();
}

Token CHTLParser::peekToken() {
    if (currentIndex_ + 1 >= tokens_.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0, 0);
    }
    return tokens_[currentIndex_ + 1];
}

void CHTLParser::ungetToken() {
    if (currentIndex_ > 0) {
        --currentIndex_;
    }
}

bool CHTLParser::isCurrentToken(TokenType type) const {
    return currentToken().type == type;
}

bool CHTLParser::isCurrentToken(const std::string& value) const {
    return currentToken().value == value;
}

bool CHTLParser::isCurrentToken(TokenType type, const std::string& value) const {
    return currentToken().type == type && currentToken().value == value;
}

Token CHTLParser::expectToken(TokenType type) {
    Token token = currentToken();
    if (token.type != type) {
        reportError("Expected token type " + std::to_string(static_cast<int>(type)) + 
                   " but got " + std::to_string(static_cast<int>(token.type)));
    }
    nextToken();
    return token;
}

Token CHTLParser::expectToken(const std::string& value) {
    Token token = currentToken();
    if (token.value != value) {
        reportError("Expected token value '" + value + "' but got '" + token.value + "'");
    }
    nextToken();
    return token;
}

Token CHTLParser::expectToken(TokenType type, const std::string& value) {
    Token token = currentToken();
    if (token.type != type || token.value != value) {
        reportError("Expected token type " + std::to_string(static_cast<int>(type)) + 
                   " with value '" + value + "' but got " + std::to_string(static_cast<int>(token.type)) + 
                   " with value '" + token.value + "'");
    }
    nextToken();
    return token;
}

void CHTLParser::skipWhitespace() {
    while (isCurrentToken(TokenType::GENERATOR_COMMENT) || 
           (currentToken().type == TokenType::IDENTIFIER && 
            (currentToken().value == " " || currentToken().value == "\t" || 
             currentToken().value == "\n" || currentToken().value == "\r"))) {
        nextToken();
    }
}

void CHTLParser::reportError(const std::string& message) {
    std::ostringstream oss;
    oss << "Parse error at line " << currentToken().line 
        << ", column " << currentToken().column << ": " << message;
    context_.addError(oss.str());
}

void CHTLParser::reportWarning(const std::string& message) {
    std::ostringstream oss;
    oss << "Parse warning at line " << currentToken().line 
        << ", column " << currentToken().column << ": " << message;
    context_.addWarning(oss.str());
}

} // namespace CHTL