#pragma once

#include "CHTLNode.h"
#include "CHTLLexer.h"
#include "CHTLVisitor.h"
#include <memory>

namespace CHTL {

// 语法分析器
class CHTLParser {
public:
    CHTLParser(std::istream& input);
    ~CHTLParser() = default;
    
    // 解析整个文档
    std::shared_ptr<CHTLNode> parse();
    
    // 解析特定类型的节点
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<TextNode> parseText();
    std::shared_ptr<StyleNode> parseStyle();
    std::shared_ptr<ScriptNode> parseScript();
    std::shared_ptr<TemplateNode> parseTemplate();
    std::shared_ptr<CustomNode> parseCustom();
    std::shared_ptr<OriginNode> parseOrigin();
    std::shared_ptr<ImportNode> parseImport();
    std::shared_ptr<ConfigNode> parseConfig();
    std::shared_ptr<NamespaceNode> parseNamespace();
    
    // 获取错误列表
    const ErrorList& getErrors() const;
    
    // 设置配置选项
    void setConfig(const ConfigOptions& config);

private:
    CHTLLexer lexer_;
    ErrorList errors_;
    ConfigOptions config_;
    
    // 当前解析的根节点
    std::shared_ptr<CHTLNode> rootNode_;
    
    // 内部解析方法
    std::shared_ptr<CHTLNode> parseNode();
    std::shared_ptr<CHTLNode> parseBlock();
    std::shared_ptr<CHTLNode> parseStatement();
    
    // 属性解析
    AttributeList parseAttributes();
    Attribute parseAttribute();
    
    // 模板解析
    std::shared_ptr<TemplateStyleNode> parseTemplateStyle();
    std::shared_ptr<TemplateElementNode> parseTemplateElement();
    std::shared_ptr<TemplateVarNode> parseTemplateVar();
    
    // 自定义解析
    std::shared_ptr<CustomStyleNode> parseCustomStyle();
    std::shared_ptr<CustomElementNode> parseCustomElement();
    std::shared_ptr<CustomVarNode> parseCustomVar();
    
    // 原始嵌入解析
    std::shared_ptr<OriginNode> parseOriginHtml();
    std::shared_ptr<OriginNode> parseOriginStyle();
    std::shared_ptr<OriginNode> parseOriginJavascript();
    
    // 导入解析
    std::shared_ptr<ImportNode> parseImportHtml();
    std::shared_ptr<ImportNode> parseImportStyle();
    std::shared_ptr<ImportNode> parseImportJavascript();
    std::shared_ptr<ImportNode> parseImportChtl();
    std::shared_ptr<ImportNode> parseImportCjmod();
    
    // 配置解析
    std::shared_ptr<ConfigNode> parseConfiguration();
    void parseConfigOptions(ConfigOptions& options);
    void parseNameGroup(ConfigOptions& options);
    
    // 命名空间解析
    std::shared_ptr<NamespaceNode> parseNamespace();
    
    // 操作符解析
    std::shared_ptr<OperatorNode> parseOperator();
    
    // 表达式解析
    std::shared_ptr<CHTLNode> parseExpression();
    std::shared_ptr<CHTLNode> parseConditionalExpression();
    std::shared_ptr<CHTLNode> parseLogicalExpression();
    std::shared_ptr<CHTLNode> parseComparisonExpression();
    std::shared_ptr<CHTLNode> parseArithmeticExpression();
    std::shared_ptr<CHTLNode> parsePrimaryExpression();
    
    // 工具方法
    bool match(TokenType type);
    bool match(const String& keyword);
    bool matchOperator(const String& op);
    Token consume(TokenType type);
    Token consume(const String& keyword);
    Token consumeOperator(const String& op);
    Token expect(TokenType type);
    Token expect(const String& keyword);
    Token expectOperator(const String& op);
    
    // 错误处理
    void addError(const String& message, const Position& pos);
    void addError(const String& message, const Token& token);
    
    // 恢复方法
    void synchronize();
    void skipToBlockEnd();
    void skipToStatementEnd();
    
    // 预读方法
    Token peek();
    Token peekNext();
    bool isAtEnd();
    
    // 语法检查
    bool isValidElementName(const String& name) const;
    bool isValidAttributeName(const String& name) const;
    bool isValidTemplateName(const String& name) const;
    bool isValidCustomName(const String& name) const;
    
    // 字符串处理
    String unescapeString(const String& str) const;
    String escapeString(const String& str) const;
    
    // 数字处理
    double parseNumber(const String& str) const;
    bool isValidNumber(const String& str) const;
};

} // namespace CHTL