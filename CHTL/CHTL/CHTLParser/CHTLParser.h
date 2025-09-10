#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTLLexer/CHTLLexer.h"
#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <memory>
#include <stack>
#include <unordered_map>

namespace CHTL {

class CHTLParser {
private:
    TokenStream tokens;
    size_t currentTokenIndex;
    
    // 解析状态
    std::stack<std::shared_ptr<BaseNode>> nodeStack;
    std::shared_ptr<BaseNode> rootNode;
    
    // 模板和自定义定义
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> templates;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> customs;
    std::unordered_map<std::string, std::string> variables;
    
    // 配置
    bool debugMode;
    bool strictMode;
    
    // 辅助方法
    Token currentToken() const;
    Token peekToken(size_t offset = 1) const;
    void advanceToken();
    bool match(TokenType type) const;
    bool match(const std::string& value) const;
    bool match(TokenType type, const std::string& value) const;
    
    // 跳过空白和注释
    void skipWhitespaceAndComments();
    
    // 错误处理
    void error(const std::string& message) const;
    void error(const std::string& message, const Token& token) const;
    void addError(const std::string& message);
    
    // 解析方法
    std::shared_ptr<BaseNode> parseDocument();
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseText();
    std::shared_ptr<BaseNode> parseComment();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseUse();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<ElementNode> element);
    std::string parseAttributeValue();
    
    // 样式解析
    std::shared_ptr<BaseNode> parseStyleBlock();
    void parseStyleProperties(std::shared_ptr<BaseNode> styleNode);
    
    // 脚本解析
    std::shared_ptr<BaseNode> parseScriptBlock();
    
    // 模板解析
    std::shared_ptr<BaseNode> parseTemplateDefinition();
    std::shared_ptr<BaseNode> parseStyleTemplate();
    std::shared_ptr<BaseNode> parseElementTemplate();
    std::shared_ptr<BaseNode> parseVariableTemplate();
    
    // 自定义解析
    std::shared_ptr<BaseNode> parseCustomDefinition();
    std::shared_ptr<BaseNode> parseCustomStyle();
    std::shared_ptr<BaseNode> parseCustomElement();
    std::shared_ptr<BaseNode> parseCustomVariable();
    
    // 特例化操作
    void parseDeleteOperation(std::shared_ptr<BaseNode> parent);
    void parseInsertOperation(std::shared_ptr<BaseNode> parent);
    
    // 原始嵌入解析
    std::shared_ptr<BaseNode> parseOriginDefinition();
    std::shared_ptr<BaseNode> parseOriginStyle();
    std::shared_ptr<BaseNode> parseOriginElement();
    std::shared_ptr<BaseNode> parseOriginJavaScript();
    
    // 导入解析
    std::shared_ptr<BaseNode> parseImportStatement();
    std::shared_ptr<BaseNode> parseImportStyle();
    std::shared_ptr<BaseNode> parseImportElement();
    std::shared_ptr<BaseNode> parseImportJavaScript();
    std::shared_ptr<BaseNode> parseImportCHTL();
    
    // 命名空间解析
    std::shared_ptr<BaseNode> parseNamespaceDefinition();
    
    // 配置解析
    std::shared_ptr<BaseNode> parseConfigurationDefinition();
    
    // use语句解析
    std::shared_ptr<BaseNode> parseUseStatement();
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseLogicalExpression();
    std::string parseEqualityExpression();
    std::string parseRelationalExpression();
    std::string parseAdditiveExpression();
    std::string parseMultiplicativeExpression();
    std::string parseUnaryExpression();
    std::string parsePrimaryExpression();
    
    // 选择器解析
    std::string parseSelector();
    
    // 路径解析
    std::string parsePath();
    
    // 局部样式块解析方法
    void parseClassSelector(std::shared_ptr<BaseNode> styleNode);
    void parseIdSelector(std::shared_ptr<BaseNode> styleNode);
    void parseContextReference(std::shared_ptr<BaseNode> styleNode);
    std::string parseStylePropertyValue();
    
    // 辅助方法
    bool isAlphaNumeric(char c) const;

public:
    CHTLParser();
    explicit CHTLParser(const TokenStream& tokens);
    
    // 设置token流
    void setTokens(const TokenStream& tokens);
    
    // 解析
    std::shared_ptr<BaseNode> parse();
    
    // 获取解析结果
    std::shared_ptr<BaseNode> getRootNode() const { return rootNode; }
    
    // 获取模板和自定义定义
    const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& getTemplates() const { return templates; }
    const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& getCustoms() const { return customs; }
    const std::unordered_map<std::string, std::string>& getVariables() const { return variables; }
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    
    // 重置解析器
    void reset();
    
    // 验证解析结果
    bool validate() const;
    
    // 获取错误信息
    std::vector<std::string> getErrors() const;

private:
    mutable std::vector<std::string> errors;
};

} // namespace CHTL

#endif // CHTL_PARSER_H