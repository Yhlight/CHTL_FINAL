#pragma once

#include "CHTLLexer.h"
#include "CHTLNode/BaseNode.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL语法分析器
 * 将词法单元序列转换为抽象语法树
 */
class CHTLParser {
public:
    CHTLParser(std::shared_ptr<CHTLLexer> lexer);
    ~CHTLParser();
    
    // 解析整个文档
    std::shared_ptr<BaseNode> parseDocument();
    
    // 解析元素
    std::shared_ptr<BaseNode> parseElement();
    
    // 解析文本节点
    std::shared_ptr<BaseNode> parseText();
    
    // 解析注释
    std::shared_ptr<BaseNode> parseComment();
    
    // 解析模板
    std::shared_ptr<BaseNode> parseTemplate();
    
    // 解析自定义
    std::shared_ptr<BaseNode> parseCustom();
    
    // 解析样式块
    std::shared_ptr<BaseNode> parseStyle();
    
    // 解析脚本块
    std::shared_ptr<BaseNode> parseScript();
    
    // 解析原始嵌入
    std::shared_ptr<BaseNode> parseOrigin();
    
    // 解析导入
    std::shared_ptr<BaseNode> parseImport();
    
    // 解析配置
    std::shared_ptr<BaseNode> parseConfig();
    
    // 解析命名空间
    std::shared_ptr<BaseNode> parseNamespace();
    
    // 解析操作符
    std::shared_ptr<BaseNode> parseOperator();
    
    // 解析属性
    void parseAttributes(std::shared_ptr<BaseNode> node);
    
    // 解析样式属性
    void parseStyleProperties(std::shared_ptr<BaseNode> node);
    
    // 解析属性条件表达式
    void parseConditionalProperties(std::shared_ptr<BaseNode> node);
    
    // 解析选择器
    void parseSelectors(std::shared_ptr<BaseNode> node);
    
    // 解析CHTL JS函数
    void parseCHTLJSFunctions(std::shared_ptr<BaseNode> node);
    
    // 错误处理
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    
    // 当前位置
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;

private:
    std::shared_ptr<CHTLLexer> m_lexer;
    std::vector<std::string> m_errors;
    
    // 当前词法单元
    Token m_currentToken;
    bool m_hasCurrentToken;
    
    // 辅助方法
    Token nextToken();
    Token peekToken();
    void consumeToken();
    bool match(TokenType type);
    bool match(const std::string& value);
    void expect(TokenType type);
    void expect(const std::string& value);
    bool isHTMLElement(const std::string& tag);
    
    // 解析辅助方法
    std::string parseString();
    std::string parseLiteral();
    std::string parseIdentifier();
    std::string parseNumber();
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseLogicalExpression();
    std::string parseComparisonExpression();
    std::string parseArithmeticExpression();
    std::string parsePrimaryExpression();
    
    // 选择器解析
    std::string parseSelector();
    std::string parseClassSelector();
    std::string parseIDSelector();
    std::string parsePseudoSelector();
    std::string parseAttributeSelector();
    
    // 模板解析
    void parseTemplateInheritance(std::shared_ptr<BaseNode> node);
    void parseTemplateVariables(std::shared_ptr<BaseNode> node);
    void parseTemplateStyles(std::shared_ptr<BaseNode> node);
    
    // 自定义解析
    void parseCustomSpecializations(std::shared_ptr<BaseNode> node);
    void parseCustomOperations(std::shared_ptr<BaseNode> node);
    
    // 导入解析
    void parseImportTarget(std::shared_ptr<BaseNode> node);
    void parseImportOptions(std::shared_ptr<BaseNode> node);
    
    // 配置解析
    void parseConfigValues(std::shared_ptr<BaseNode> node);
    void parseNameConfigs(std::shared_ptr<BaseNode> node);
    void parseGroupOptions(std::shared_ptr<BaseNode> node);
    
    // 命名空间解析
    void parseNamespaceContent(std::shared_ptr<BaseNode> node);
    void parseNamespaceConstraints(std::shared_ptr<BaseNode> node);
    
    // 操作符解析
    void parseDeleteOperation(std::shared_ptr<BaseNode> node);
    void parseInsertOperation(std::shared_ptr<BaseNode> node);
    void parseInheritOperation(std::shared_ptr<BaseNode> node);
    void parseUseOperation(std::shared_ptr<BaseNode> node);
};

} // namespace CHTL