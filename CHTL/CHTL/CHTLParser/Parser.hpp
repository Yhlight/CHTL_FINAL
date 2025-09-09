#pragma once

#include <CHTL/CHTLLexer/Lexer.hpp>
#include <CHTL/CHTLLexer/Token.hpp>
#include <CHTL/CHTLNode/BaseNode.hpp>
#include <CHTL/CHTLNode/ElementNode.hpp>
#include <CHTL/CHTLNode/TextNode.hpp>
#include <CHTL/CHTLNode/StyleNode.hpp>
#include <CHTL/CHTLNode/ScriptNode.hpp>
#include <CHTL/CHTLNode/TemplateNode.hpp>
#include <CHTL/CHTLNode/TemplateStyleNode.hpp>
#include <CHTL/CHTLNode/TemplateElementNode.hpp>
#include <CHTL/CHTLNode/TemplateVarNode.hpp>
#include <CHTL/CHTLNode/CustomNode.hpp>
#include <CHTL/CHTLNode/CustomStyleNode.hpp>
#include <CHTL/CHTLNode/CustomElementNode.hpp>
#include <CHTL/CHTLNode/CustomVarNode.hpp>
#include <CHTL/CHTLNode/OriginNode.hpp>
#include <CHTL/CHTLNode/ImportNode.hpp>
#include <CHTL/CHTLNode/ConfigNode.hpp>
#include <CHTL/CHTLNode/NamespaceNode.hpp>
#include <CHTL/CHTLNode/ConstraintNode.hpp>
#include <CHTL/CHTLManage/TemplateManager.hpp>
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

/**
 * @brief CHTL 语法分析器
 * 
 * 根据 CHTL.md 文档第 52-258 行定义的语法规范实现
 * 支持：
 * - 元素节点解析
 * - 属性解析
 * - 局部样式块解析
 * - 模板解析
 * - 自定义解析
 */
class Parser {
public:
    explicit Parser(const std::string& source);
    explicit Parser(Lexer& lexer);
    
    // 解析整个文档
    std::shared_ptr<BaseNode> parse();
    
    // 解析元素
    std::shared_ptr<ElementNode> parseElement();
    
    // 解析文本节点
    std::shared_ptr<TextNode> parseText();
    
    // 解析样式块
    std::shared_ptr<StyleNode> parseStyle();
    
    // 解析脚本块
    std::shared_ptr<BaseNode> parseScript();
    
    // 解析模板
    std::shared_ptr<BaseNode> parseTemplate();
    
    // 解析自定义
    std::shared_ptr<BaseNode> parseCustom();
    
    // 解析原始嵌入
    std::shared_ptr<BaseNode> parseOrigin();
    
    // 解析导入
    std::shared_ptr<BaseNode> parseImport();
    
    // 解析配置
    std::shared_ptr<BaseNode> parseConfig();
    
    // 解析命名空间
    std::shared_ptr<BaseNode> parseNamespace();
    
    // 获取当前词法单元
    Token currentToken() const;
    
    // 获取下一个词法单元
    Token nextToken();
    
    // 查看下一个词法单元
    Token peekToken();
    
    // 检查是否有错误
    bool hasError() const { return hasError_; }
    
    // 获取错误信息
    const std::string& getError() const { return errorMessage_; }
    
private:
    // 匹配指定类型的词法单元
    bool match(TokenType type);
    
    // 匹配指定类型的词法单元并消费
    bool consume(TokenType type);
    
    // 匹配指定值的词法单元
    bool matchValue(const std::string& value);
    
    // 匹配指定值的词法单元并消费
    bool consumeValue(const std::string& value);
    
    // 解析属性
    void parseAttributes(ElementNode* element);
    
    // 解析属性值
    std::string parseAttributeValue();
    
    // 解析样式属性
    void parseStyleProperties(StyleNode* style);
    
    // 解析全局样式规则
    void parseGlobalStyleRules(StyleNode* style);
    
    // 解析选择器
    std::string parseSelector();
    
    // 解析模板类型
    std::string parseTemplateType();
    
    // 解析自定义类型
    std::string parseCustomType();
    
    // 解析原始嵌入类型
    std::string parseOriginType();
    
    // 解析导入类型
    std::string parseImportType();
    
    // 解析配置类型
    std::string parseConfigType();
    
    // 解析命名空间类型
    std::string parseNamespaceType();
    
    // 解析样式模板内容
    void parseStyleTemplateContent(TemplateStyleNode* styleTemplate);
    
    // 解析变量模板内容
    void parseVarTemplateContent(TemplateVarNode* varTemplate);
    
    // 解析模板继承
    void parseTemplateInheritance(BaseNode* template_node);
    
    // 解析自定义样式组内容
    void parseCustomStyleContent(CustomStyleNode* customStyle);
    
    // 解析自定义元素内容
    void parseCustomElementContent(CustomElementNode* customElement);
    
    // 解析自定义变量组内容
    void parseCustomVarContent(CustomVarNode* customVar);
    
    // 解析删除操作
    void parseDeleteOperation(BaseNode* customNode);
    
    // 解析插入操作
    void parseInsertOperation(CustomElementNode* customElement);
    
    // 解析 Name 块
    void parseNameBlock(ConfigNode* config);
    
    // 解析 Use 语句
    std::shared_ptr<BaseNode> parseUse();
    
    // 解析约束
    std::shared_ptr<BaseNode> parseConstraint();
    
    // 解析 except 约束
    void parseExceptConstraint(ConstraintNode* constraint);
    
    // CHTL JS 解析方法
    void parseFileLoader(ScriptNode* script);
    void parseListen(ScriptNode* script);
    void parseDelegate(ScriptNode* script);
    void parseAnimate(ScriptNode* script);
    void parseVir(ScriptNode* script);
    void parseRouter(ScriptNode* script);
    void parseUtil(ScriptNode* script);
    void parseINeverAway(ScriptNode* script);
    void parsePrintMylove(ScriptNode* script);
    
    // 报告错误
    void reportError(const std::string& message);
    
    // 同步到下一个语句
    void synchronize();
    
    // 检查是否为语句开始
    bool isStatementStart(TokenType type);
    
    // 检查是否为元素开始
    bool isElementStart(TokenType type);
    
    // 检查是否为模板开始
    bool isTemplateStart(TokenType type);
    
    // 检查是否为自定义开始
    bool isCustomStart(TokenType type);
    
    // 检查是否为原始嵌入开始
    bool isOriginStart(TokenType type);
    
    // 检查是否为导入开始
    bool isImportStart(TokenType type);
    
    // 检查是否为配置开始
    bool isConfigStart(TokenType type);
    
    // 检查是否为命名空间开始
    bool isNamespaceStart(TokenType type);
    
private:
    Lexer lexer_;
    Token current_;
    bool hasError_;
    std::string errorMessage_;
};

} // namespace CHTL