#pragma once

#include "CHTLNode.h"
#include "CHTLVisitor.h"
#include <ostream>
#include <unordered_map>

namespace CHTL {

// 代码生成器基类
class CHTLGenerator : public CHTLVisitor {
public:
    CHTLGenerator(std::ostream& output);
    virtual ~CHTLGenerator() = default;
    
    // 生成代码
    virtual void generate(std::shared_ptr<CHTLNode> root) = 0;
    
    // 设置配置选项
    void setConfig(const ConfigOptions& config);
    
    // 获取错误列表
    const ErrorList& getErrors() const;

protected:
    std::ostream& output_;
    ConfigOptions config_;
    ErrorList errors_;
    
    // 缩进管理
    int indentLevel_ = 0;
    String indentString_ = "    ";
    
    void increaseIndent();
    void decreaseIndent();
    void writeIndent();
    
    // 错误处理
    void addError(const String& message, const Position& pos);
    
    // 工具方法
    String escapeHtml(const String& str) const;
    String escapeCss(const String& str) const;
    String escapeJs(const String& str) const;
    String formatAttribute(const Attribute& attr) const;
    String formatAttributes(const AttributeList& attrs) const;
};

// HTML 生成器
class HTMLGenerator : public CHTLGenerator {
public:
    HTMLGenerator(std::ostream& output);
    ~HTMLGenerator() = default;
    
    void generate(std::shared_ptr<CHTLNode> root) override;
    
    // 节点访问
    void visit(TextNode& node) override;
    void visit(ElementNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;
    void visit(TemplateVarNode& node) override;
    void visit(CustomStyleNode& node) override;
    void visit(CustomElementNode& node) override;
    void visit(CustomVarNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    void generateElement(ElementNode& node);
    void generateText(TextNode& node);
    void generateComment(CommentNode& node);
    void generateStyle(StyleNode& node);
    void generateScript(ScriptNode& node);
    void generateTemplate(TemplateNode& node);
    void generateCustom(CustomNode& node);
    void generateOrigin(OriginNode& node);
    void generateImport(ImportNode& node);
    
    // HTML 特定方法
    bool isVoidElement(const String& tagName) const;
    bool isBlockElement(const String& tagName) const;
    bool isInlineElement(const String& tagName) const;
    String formatTagName(const String& name) const;
    String formatAttributeValue(const String& value) const;
    
    // 模板处理
    void processTemplate(TemplateNode& node);
    void processCustom(CustomNode& node);
    void processInheritance(TemplateNode& node);
    void processSpecialization(CustomNode& node);
    
    // 变量替换
    String replaceVariables(const String& content) const;
    String getVariableValue(const String& name) const;
    
    // 命名空间处理
    String resolveNamespace(const String& name) const;
    void processNamespace(NamespaceNode& node);
    
    // 导入处理
    void processImport(ImportNode& node);
    void loadImportedFile(const String& path);
    
    // 配置处理
    void processConfig(ConfigNode& node);
    
    // 状态管理
    std::unordered_map<String, String> variables_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> templates_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> customs_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> namespaces_;
    std::vector<std::shared_ptr<CHTLNode>> imports_;
};

// CSS 生成器
class CSSGenerator : public CHTLGenerator {
public:
    CSSGenerator(std::ostream& output);
    ~CSSGenerator() = default;
    
    void generate(std::shared_ptr<CHTLNode> root) override;
    
    // 节点访问
    void visit(TextNode& node) override;
    void visit(ElementNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;
    void visit(TemplateVarNode& node) override;
    void visit(CustomStyleNode& node) override;
    void visit(CustomElementNode& node) override;
    void visit(CustomVarNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    void generateStyle(StyleNode& node);
    void generateTemplateStyle(TemplateStyleNode& node);
    void generateCustomStyle(CustomStyleNode& node);
    void generateOriginStyle(OriginNode& node);
    
    // CSS 特定方法
    String formatCssProperty(const String& name, const String& value) const;
    String formatCssSelector(const String& selector) const;
    String formatCssValue(const String& value) const;
    
    // 选择器处理
    void processSelectors(StyleNode& node);
    void processClassSelectors(StyleNode& node);
    void processIdSelectors(StyleNode& node);
    void processPseudoSelectors(StyleNode& node);
    
    // 条件表达式处理
    String processConditionalExpression(const String& expr) const;
    String processLogicalExpression(const String& expr) const;
    String processComparisonExpression(const String& expr) const;
    String processArithmeticExpression(const String& expr) const;
    
    // 变量处理
    String resolveVariable(const String& name) const;
    void processVariables(TemplateVarNode& node);
    
    // 继承处理
    void processInheritance(TemplateStyleNode& node);
    void processSpecialization(CustomStyleNode& node);
    
    // 命名空间处理
    String resolveNamespace(const String& name) const;
    
    // 状态管理
    std::unordered_map<String, String> variables_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> templates_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> customs_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> namespaces_;
};

// JavaScript 生成器
class JSGenerator : public CHTLGenerator {
public:
    JSGenerator(std::ostream& output);
    ~JSGenerator() = default;
    
    void generate(std::shared_ptr<CHTLNode> root) override;
    
    // 节点访问
    void visit(TextNode& node) override;
    void visit(ElementNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;
    void visit(TemplateVarNode& node) override;
    void visit(CustomStyleNode& node) override;
    void visit(CustomElementNode& node) override;
    void visit(CustomVarNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    void generateScript(ScriptNode& node);
    void generateOriginScript(OriginNode& node);
    
    // JavaScript 特定方法
    String formatJsExpression(const String& expr) const;
    String formatJsFunction(const String& name, const String& params, const String& body) const;
    String formatJsObject(const String& obj) const;
    String formatJsArray(const String& arr) const;
    
    // 选择器处理
    String processSelector(const String& selector) const;
    String processClassSelector(const String& className) const;
    String processIdSelector(const String& id) const;
    String processTagSelector(const String& tagName) const;
    
    // 事件处理
    void processEventListeners(ScriptNode& node);
    void processEventDelegation(ScriptNode& node);
    
    // 动画处理
    void processAnimations(ScriptNode& node);
    
    // 路由处理
    void processRouting(ScriptNode& node);
    
    // 虚对象处理
    void processVirtualObjects(ScriptNode& node);
    
    // 变量处理
    String resolveVariable(const String& name) const;
    
    // 状态管理
    std::unordered_map<String, String> variables_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> templates_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> customs_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> namespaces_;
};

} // namespace CHTL