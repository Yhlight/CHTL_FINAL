#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTLASTNode.h"
#include "../CHTLLexer/CHTLToken.h"
#include "../CHTLLexer/CHTLLexer.h"
#include <memory>
#include <vector>

namespace CHTL {

class CHTLParser {
private:
    CHTLTokenList tokens;
    size_t currentIndex;
    
    CHTLToken getCurrent() const;
    CHTLToken peek(size_t offset = 1) const;
    bool hasNext() const;
    bool hasNext(size_t offset) const;
    void advance();
    bool match(CHTLTokenType type) const;
    bool match(const std::vector<CHTLTokenType>& types) const;
    CHTLToken consume(CHTLTokenType type, const std::string& message);
    bool check(CHTLTokenType type) const;
    bool isAtEnd() const;
    
    void error(const std::string& message) const;
    void synchronize();
    
    // 解析方法
    std::shared_ptr<CHTLASTNode> parseProgram();
    std::shared_ptr<CHTLASTNode> parseStatement();
    std::shared_ptr<CHTLASTNode> parseHTMLElement();
    std::shared_ptr<CHTLASTNode> parseTextNode();
    std::shared_ptr<CHTLASTNode> parseAttribute();
    std::shared_ptr<CHTLASTNode> parseStyleBlock();
    std::shared_ptr<CHTLASTNode> parseStyleRule();
    std::shared_ptr<CHTLASTNode> parseStyleProperty();
    std::shared_ptr<CHTLASTNode> parseScriptBlock();
    
    // 模板解析
    std::shared_ptr<CHTLASTNode> parseTemplateDeclaration();
    std::shared_ptr<CHTLASTNode> parseCustomDeclaration();
    std::shared_ptr<CHTLASTNode> parseOriginNode();
    std::shared_ptr<CHTLASTNode> parseImportNode();
    std::shared_ptr<CHTLASTNode> parseNamespaceNode();
    std::shared_ptr<CHTLASTNode> parseConfigurationNode();
    std::shared_ptr<CHTLASTNode> parseInfoNode();
    std::shared_ptr<CHTLASTNode> parseExportNode();
    
    // 操作解析
    std::shared_ptr<CHTLASTNode> parseDeleteNode();
    std::shared_ptr<CHTLASTNode> parseInsertNode();
    std::shared_ptr<CHTLASTNode> parseInheritNode();
    std::shared_ptr<CHTLASTNode> parseExceptNode();
    
    // 辅助方法
    std::vector<std::shared_ptr<CHTLASTNode>> parseAttributeList();
    std::vector<std::shared_ptr<CHTLASTNode>> parseStyleRules();
    std::vector<std::shared_ptr<CHTLASTNode>> parseStyleProperties();
    std::string parseStringLiteral();
    std::string parseUnquotedLiteral();
    
    // 工具方法
    bool isHTMLElement(const std::string& name) const;
    bool isSelfClosingElement(const std::string& name) const;
    std::string getOperatorString(CHTLTokenType type) const;
    
public:
    CHTLParser(const CHTLTokenList& tokenList);
    CHTLParser(const std::string& source);
    ~CHTLParser() = default;
    
    std::shared_ptr<CHTLASTNode> parse();
    std::shared_ptr<CHTLASTNode> parse(const std::string& source);
    
    void reset();
    void setTokens(const CHTLTokenList& tokenList);
    
    CHTLTokenList getTokens() const;
    size_t getCurrentIndex() const;
    bool hasErrors() const;
};

} // namespace CHTL

#endif // CHTL_PARSER_H