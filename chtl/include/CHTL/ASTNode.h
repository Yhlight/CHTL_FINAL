#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {

/**
 * AST节点类型枚举
 */
enum class ASTNodeType {
    // 基础节点
    ROOT,           // 根节点
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    ATTRIBUTE,      // 属性节点
    COMMENT,        // 注释节点
    
    // 样式相关
    STYLE_BLOCK,    // 样式块
    STYLE_PROPERTY, // 样式属性
    STYLE_SELECTOR, // 样式选择器
    STYLE_VALUE,    // 样式值
    STYLE_RULE,     // 样式规则
    STYLE_DECLARATION, // 样式声明
    
    // 脚本相关
    SCRIPT_BLOCK,   // 脚本块
    SCRIPT_STATEMENT, // 脚本语句
    
    // 模板相关
    TEMPLATE,       // 模板
    TEMPLATE_STYLE, // 样式模板
    TEMPLATE_ELEMENT, // 元素模板
    TEMPLATE_VAR,   // 变量模板
    
    // 自定义相关
    CUSTOM,         // 自定义
    CUSTOM_STYLE,   // 自定义样式
    CUSTOM_ELEMENT, // 自定义元素
    CUSTOM_VAR,     // 自定义变量
    
    // 原始嵌入
    ORIGIN,         // 原始嵌入
    ORIGIN_HTML,    // HTML原始嵌入
    ORIGIN_CSS,     // CSS原始嵌入
    ORIGIN_JS,      // JS原始嵌入
    
    // 导入和命名空间
    IMPORT,         // 导入
    NAMESPACE,      // 命名空间
    
    // 配置
    CONFIGURATION,  // 配置
    CONFIG_ITEM,    // 配置项
    CONFIG_NAME,    // Name配置块
    CONFIG_ORIGIN_TYPE, // OriginType配置块
    
    // 约束
    CONSTRAINT,     // 约束
    EXCEPT,         // except约束
    
    // 使用
    USE,            // use语句
    
    // 特例化操作
    DELETE,         // delete操作
    INSERT,         // insert操作
    INHERIT,        // inherit操作
    
    // 表达式
    EXPRESSION,     // 表达式
    BINARY_OP,      // 二元操作
    UNARY_OP,       // 一元操作
    CONDITIONAL,    // 条件表达式
    REFERENCE,      // 引用表达式
    
    // 选择器
    SELECTOR,       // 选择器
    CLASS_SELECTOR, // 类选择器
    ID_SELECTOR,    // ID选择器
    TAG_SELECTOR,   // 标签选择器
    PSEUDO_SELECTOR, // 伪选择器
    
    // CHTL JS相关
    CHTLJS_FUNCTION, // CHTL JS函数
    CHTLJS_OBJECT,   // CHTL JS对象
    CHTLJS_ARRAY,    // CHTL JS数组
    CHTLJS_VIR,      // 虚对象
    CHTLJS_LISTEN,   // 监听器
    CHTLJS_DELEGATE, // 事件委托
    CHTLJS_ANIMATE,  // 动画
    CHTLJS_ROUTER,   // 路由
    CHTLJS_FILELOADER, // 文件加载器
};

/**
 * 属性值类型
 */
using AttributeValue = std::variant<std::string, double, bool>;

/**
 * AST节点基类
 */
class ASTNode {
public:
    ASTNode(ASTNodeType type, const std::string& name = "");
    virtual ~ASTNode() = default;
    
    // 基本信息
    ASTNodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 子节点管理
    void addChild(std::shared_ptr<ASTNode> child);
    void removeChild(std::shared_ptr<ASTNode> child);
    const std::vector<std::shared_ptr<ASTNode>>& getChildren() const { return children_; }
    std::vector<std::shared_ptr<ASTNode>>& getChildren() { return children_; }
    
    // 属性管理
    void setAttribute(const std::string& key, const AttributeValue& value);
    AttributeValue getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, AttributeValue>& getAttributes() const { return attributes_; }
    
    // 文本内容
    const std::string& getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; }
    
    // 访问者模式
    virtual void accept(class ASTVisitor& visitor) = 0;
    
    // 调试信息
    std::string toString() const;
    std::string toTreeString(int indent = 0) const;
    
protected:
    ASTNodeType type_;
    std::string name_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<ASTNode>> children_;
    std::unordered_map<std::string, AttributeValue> attributes_;
    std::string text_;
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getTagName() const { return getName(); }
    void setTagName(const std::string& tagName) { setName(tagName); }
    
    bool isSelfClosing() const;
    void setSelfClosing(bool selfClosing) { selfClosing_ = selfClosing; }
    
private:
    bool selfClosing_;
};

/**
 * 文本节点
 */
class TextNode : public ASTNode {
public:
    TextNode(const std::string& content);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getContent() const { return getText(); }
    void setContent(const std::string& content) { setText(content); }
};

/**
 * 样式节点
 */
class StyleNode : public ASTNode {
public:
    StyleNode();
    void accept(ASTVisitor& visitor) override;
    
    void addProperty(const std::string& property, const std::string& value);
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties_; }
    
private:
    std::unordered_map<std::string, std::string> properties_;
};

/**
 * 脚本节点
 */
class ScriptNode : public ASTNode {
public:
    ScriptNode();
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getScript() const { return getText(); }
    void setScript(const std::string& script) { setText(script); }
};

/**
 * 模板节点
 */
class TemplateNode : public ASTNode {
public:
    TemplateNode(const std::string& templateType, const std::string& templateName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getTemplateType() const { return templateType_; }
    const std::string& getTemplateName() const { return templateName_; }
    
private:
    std::string templateType_;
    std::string templateName_;
};

/**
 * 自定义节点
 */
class CustomNode : public ASTNode {
public:
    CustomNode(const std::string& customType, const std::string& customName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getCustomType() const { return customType_; }
    const std::string& getCustomName() const { return customName_; }
    
private:
    std::string customType_;
    std::string customName_;
};

/**
 * 原始嵌入节点
 */
class OriginNode : public ASTNode {
public:
    OriginNode(const std::string& originType, const std::string& originName = "");
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getOriginType() const { return originType_; }
    const std::string& getOriginName() const { return originName_; }
    
private:
    std::string originType_;
    std::string originName_;
};

/**
 * 导入节点
 */
class ImportNode : public ASTNode {
public:
    ImportNode(const std::string& importType, const std::string& importPath);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getImportType() const { return importType_; }
    const std::string& getImportPath() const { return importPath_; }
    const std::string& getImportAlias() const { return importAlias_; }
    void setImportAlias(const std::string& alias) { importAlias_ = alias; }
    
private:
    std::string importType_;
    std::string importPath_;
    std::string importAlias_;
};

/**
 * 命名空间节点
 */
class NamespaceNode : public ASTNode {
public:
    NamespaceNode(const std::string& namespaceName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getNamespaceName() const { return getName(); }
};

/**
 * 配置节点
 */
class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode(const std::string& configName = "");
    void accept(ASTVisitor& visitor) override;
    
    void addConfigItem(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getConfigItems() const { return configItems_; }
    
private:
    std::unordered_map<std::string, std::string> configItems_;
};

/**
 * 约束节点
 */
class ConstraintNode : public ASTNode {
public:
    ConstraintNode();
    void accept(ASTVisitor& visitor) override;
    
    void addException(const std::string& exception);
    const std::vector<std::string>& getExceptions() const { return exceptions_; }
    
private:
    std::vector<std::string> exceptions_;
};

/**
 * 使用节点
 */
class UseNode : public ASTNode {
public:
    UseNode(const std::string& useTarget);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getUseTarget() const { return getText(); }
};

/**
 * 样式规则节点
 */
class StyleRuleNode : public ASTNode {
public:
    StyleRuleNode();
    void accept(ASTVisitor& visitor) override;
    
    void addSelector(const std::string& selector);
    void addDeclaration(const std::string& property, const std::string& value);
    
    const std::vector<std::string>& getSelectors() const { return selectors_; }
    const std::unordered_map<std::string, std::string>& getDeclarations() const { return declarations_; }
    
    // 兼容性方法
    const std::unordered_map<std::string, std::string>& getProperties() const { return declarations_; }
    
private:
    std::vector<std::string> selectors_;
    std::unordered_map<std::string, std::string> declarations_;
};

/**
 * 表达式类型枚举
 */
enum class ExpressionType {
    LITERAL,
    IDENTIFIER,
    BINARY_OP,
    CONDITIONAL,
    REFERENCE
};

/**
 * 表达式节点
 */
class ExpressionNode : public ASTNode {
public:
    ExpressionNode(const std::string& expression);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getExpression() const { return getText(); }
    void setExpression(const std::string& expression) { setText(expression); }
    
    // 新增方法
    ExpressionType getExpressionType() const { return expressionType_; }
    void setExpressionType(ExpressionType type) { expressionType_ = type; }
    const std::string& getLiteralValue() const { return getText(); }
    const std::string& getIdentifierName() const { return getText(); }
    
private:
    ExpressionType expressionType_;
};

/**
 * 二元操作节点
 */
class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(const std::string& operator_, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getOperatorString() const { return getText(); }
    std::shared_ptr<ASTNode> getLeft() const;
    std::shared_ptr<ASTNode> getRight() const;
    
    // 新增方法
    TokenType getOperator() const { return operator_; }
    
private:
    TokenType operator_;
};

/**
 * 条件表达式节点
 */
class ConditionalNode : public ASTNode {
public:
    ConditionalNode(std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> trueExpr, std::shared_ptr<ASTNode> falseExpr);
    void accept(ASTVisitor& visitor) override;
    
    std::shared_ptr<ASTNode> getCondition() const;
    std::shared_ptr<ASTNode> getTrueExpression() const;
    std::shared_ptr<ASTNode> getFalseExpression() const;
    
    // 兼容性方法
    std::shared_ptr<ASTNode> getTrueExpr() const { return getTrueExpression(); }
    std::shared_ptr<ASTNode> getFalseExpr() const { return getFalseExpression(); }
};

/**
 * 引用表达式节点
 */
class ReferenceNode : public ASTNode {
public:
    ReferenceNode(const std::string& selector, const std::string& property);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getSelector() const { return selector_; }
    const std::string& getProperty() const { return property_; }
    
    // 兼容性方法
    const std::string& getReference() const { return getText(); }
    
private:
    std::string selector_;
    std::string property_;
};

/**
 * 选择器类型枚举
 */
enum class SelectorType {
    TAG,
    CLASS,
    ID,
    PSEUDO
};

/**
 * 选择器节点
 */
class SelectorNode : public ASTNode {
public:
    SelectorNode(const std::string& selector);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getSelector() const { return getText(); }
    void setSelector(const std::string& selector) { setText(selector); }
    
    // 新增方法
    SelectorType getSelectorType() const { return selectorType_; }
    void setSelectorType(SelectorType type) { selectorType_ = type; }
    const std::string& getTagName() const { return getText(); }
    const std::string& getClassName() const { return getText(); }
    const std::string& getIdName() const { return getText(); }
    const std::string& getPseudoName() const { return getText(); }
    const std::string& getSelectorValue() const { return getText(); }
    
private:
    SelectorType selectorType_;
};

/**
 * CHTL JS函数节点
 */
class CHTLJSFunctionNode : public ASTNode {
public:
    CHTLJSFunctionNode(const std::string& functionName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getFunctionName() const { return getName(); }
    void addParameter(const std::string& param);
    const std::vector<std::string>& getParameters() const { return parameters_; }
    
    // 兼容性方法
    const std::string& getFunctionBody() const { return getText(); }
    void setFunctionBody(const std::string& body) { setText(body); }
    
private:
    std::vector<std::string> parameters_;
};

/**
 * 虚对象节点
 */
class CHTLJSVirNode : public ASTNode {
public:
    CHTLJSVirNode(const std::string& virName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getVirName() const { return getName(); }
    
    // 兼容性方法
    const std::string& getVirBody() const { return getText(); }
    void setVirBody(const std::string& body) { setText(body); }
};

} // namespace CHTL