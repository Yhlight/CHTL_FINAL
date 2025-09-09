#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>
#include <stack>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(const std::string& source);
    
    // 解析整个文档
    std::shared_ptr<BaseNode> parse();
    
    // 解析特定节点类型
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseConfiguration();
    
private:
    std::unique_ptr<Lexer> lexer_;
    std::stack<std::shared_ptr<BaseNode>> node_stack_;
    
    // 辅助方法
    std::unique_ptr<Token> currentToken();
    std::unique_ptr<Token> nextToken();
    std::unique_ptr<Token> peekToken();
    bool match(TokenType type);
    bool match(const std::string& value);
    void consume(TokenType type);
    void consume(const std::string& value);
    void skipTo(TokenType type);
    
    // 解析辅助方法
    std::string parseIdentifier();
    std::string parseString();
    std::string parseLiteral();
    void parseAttributes(std::shared_ptr<BaseNode> node);
    void parseStyleBlock(std::shared_ptr<BaseNode> node);
    void parseScriptBlock(std::shared_ptr<BaseNode> node);
    
    // 错误处理
    void error(const std::string& message);
    void expect(TokenType type);
    void expect(const std::string& value);
    
    // 节点创建辅助方法
    std::shared_ptr<BaseNode> createElementNode(const std::string& tagName);
    std::shared_ptr<BaseNode> createTextNode(const std::string& content);
    std::shared_ptr<BaseNode> createTemplateNode(NodeType type, const std::string& name);
    std::shared_ptr<BaseNode> createCustomNode(NodeType type, const std::string& name);
};

} // namespace CHTL