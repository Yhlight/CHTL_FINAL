#ifndef CHTLASTNODE_H
#define CHTLASTNODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

/**
 * CHTL AST节点基类
 */
class CHTLASTNode {
public:
    enum class NodeType {
        // 基础节点
        PROGRAM,
        ELEMENT,
        TEXT,
        ATTRIBUTE,
        COMMENT,
        
        // 样式相关
        STYLE_BLOCK,
        STYLE_PROPERTY,
        STYLE_SELECTOR,
        STYLE_RULE,
        
        // 脚本相关
        SCRIPT_BLOCK,
        
        // 模板相关
        TEMPLATE_STYLE,
        TEMPLATE_ELEMENT,
        TEMPLATE_VAR,
        
        // 自定义相关
        CUSTOM_STYLE,
        CUSTOM_ELEMENT,
        CUSTOM_VAR,
        
        // 原始嵌入
        ORIGIN_HTML,
        ORIGIN_STYLE,
        ORIGIN_JAVASCRIPT,
        
        // 导入
        IMPORT_HTML,
        IMPORT_STYLE,
        IMPORT_JAVASCRIPT,
        IMPORT_CHTL,
        IMPORT_CJMOD,
        
        // 配置
        CONFIGURATION,
        CONFIG_PROPERTY,
        CONFIG_NAME_GROUP,
        
        // 命名空间
        NAMESPACE,
        
        // 信息
        INFO,
        EXPORT,
        
        // 操作符
        DELETE_OPERATION,
        INSERT_OPERATION,
        INHERIT_OPERATION,
        
        // 表达式
        EXPRESSION,
        BINARY_EXPRESSION,
        UNARY_EXPRESSION,
        CONDITIONAL_EXPRESSION,
        PROPERTY_REFERENCE,
        FUNCTION_CALL,
        
        // 字面量
        LITERAL,
        IDENTIFIER,
        
        // 特殊
        USE_STATEMENT,
        EXCEPT_CLAUSE
    };

    CHTLASTNode(NodeType type);
    virtual ~CHTLASTNode() = default;

    NodeType getType() const;
    virtual std::string toString() const = 0;
    virtual std::string generateCode() const = 0;

protected:
    NodeType type;
};

/**
 * 程序节点
 */
class ProgramNode : public CHTLASTNode {
public:
    ProgramNode();
    void addStatement(std::shared_ptr<CHTLASTNode> statement);
    std::vector<std::shared_ptr<CHTLASTNode>> getStatements() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::vector<std::shared_ptr<CHTLASTNode>> statements;
};

/**
 * 元素节点
 */
class ElementNode : public CHTLASTNode {
public:
    ElementNode(const std::string& tagName);
    
    void setTagName(const std::string& tagName);
    std::string getTagName() const;
    
    void addAttribute(std::shared_ptr<CHTLASTNode> attribute);
    std::vector<std::shared_ptr<CHTLASTNode>> getAttributes() const;
    
    void addChild(std::shared_ptr<CHTLASTNode> child);
    std::vector<std::shared_ptr<CHTLASTNode>> getChildren() const;
    
    void setStyleBlock(std::shared_ptr<CHTLASTNode> styleBlock);
    std::shared_ptr<CHTLASTNode> getStyleBlock() const;
    
    void setScriptBlock(std::shared_ptr<CHTLASTNode> scriptBlock);
    std::shared_ptr<CHTLASTNode> getScriptBlock() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string tagName;
    std::vector<std::shared_ptr<CHTLASTNode>> attributes;
    std::vector<std::shared_ptr<CHTLASTNode>> children;
    std::shared_ptr<CHTLASTNode> styleBlock;
    std::shared_ptr<CHTLASTNode> scriptBlock;
};

/**
 * 文本节点
 */
class TextNode : public CHTLASTNode {
public:
    TextNode(const std::string& content);
    
    void setContent(const std::string& content);
    std::string getContent() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string content;
};

/**
 * 属性节点
 */
class AttributeNode : public CHTLASTNode {
public:
    AttributeNode(const std::string& name, std::shared_ptr<CHTLASTNode> value);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void setValue(std::shared_ptr<CHTLASTNode> value);
    std::shared_ptr<CHTLASTNode> getValue() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::shared_ptr<CHTLASTNode> value;
};

/**
 * 样式块节点
 */
class StyleBlockNode : public CHTLASTNode {
public:
    StyleBlockNode();
    
    void addRule(std::shared_ptr<CHTLASTNode> rule);
    std::vector<std::shared_ptr<CHTLASTNode>> getRules() const;
    
    void addProperty(std::shared_ptr<CHTLASTNode> property);
    std::vector<std::shared_ptr<CHTLASTNode>> getProperties() const;
    
    void setInline(bool inline_);
    bool isInline() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::vector<std::shared_ptr<CHTLASTNode>> rules;
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
    bool inline_;
};

/**
 * 样式规则节点
 */
class StyleRuleNode : public CHTLASTNode {
public:
    StyleRuleNode(std::shared_ptr<CHTLASTNode> selector);
    
    void setSelector(std::shared_ptr<CHTLASTNode> selector);
    std::shared_ptr<CHTLASTNode> getSelector() const;
    
    void addProperty(std::shared_ptr<CHTLASTNode> property);
    std::vector<std::shared_ptr<CHTLASTNode>> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<CHTLASTNode> selector;
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
};

/**
 * 样式选择器节点
 */
class StyleSelectorNode : public CHTLASTNode {
public:
    StyleSelectorNode(const std::string& selector);
    
    void setSelector(const std::string& selector);
    std::string getSelector() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string selector;
};

/**
 * 样式属性节点
 */
class StylePropertyNode : public CHTLASTNode {
public:
    StylePropertyNode(const std::string& name, std::shared_ptr<CHTLASTNode> value);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void setValue(std::shared_ptr<CHTLASTNode> value);
    std::shared_ptr<CHTLASTNode> getValue() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::shared_ptr<CHTLASTNode> value;
};

/**
 * 脚本块节点
 */
class ScriptBlockNode : public CHTLASTNode {
public:
    ScriptBlockNode(const std::string& content);
    
    void setContent(const std::string& content);
    std::string getContent() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string content;
};

/**
 * 模板节点基类
 */
class TemplateNode : public CHTLASTNode {
public:
    TemplateNode(NodeType type, const std::string& name);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void addProperty(std::shared_ptr<CHTLASTNode> property);
    std::vector<std::shared_ptr<CHTLASTNode>> getProperties() const;
    
    void addChild(std::shared_ptr<CHTLASTNode> child);
    std::vector<std::shared_ptr<CHTLASTNode>> getChildren() const;

protected:
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
    std::vector<std::shared_ptr<CHTLASTNode>> children;
};

/**
 * 样式组模板节点
 */
class TemplateStyleNode : public TemplateNode {
public:
    TemplateStyleNode(const std::string& name);
    
    std::string toString() const override;
    std::string generateCode() const override;
};

/**
 * 元素模板节点
 */
class TemplateElementNode : public TemplateNode {
public:
    TemplateElementNode(const std::string& name);
    
    std::string toString() const override;
    std::string generateCode() const override;
};

/**
 * 变量组模板节点
 */
class TemplateVarNode : public TemplateNode {
public:
    TemplateVarNode(const std::string& name);
    
    std::string toString() const override;
    std::string generateCode() const override;
};

/**
 * 自定义节点基类
 */
class CustomNode : public CHTLASTNode {
public:
    CustomNode(NodeType type, const std::string& name);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void addProperty(std::shared_ptr<CHTLASTNode> property);
    std::vector<std::shared_ptr<CHTLASTNode>> getProperties() const;
    
    void addChild(std::shared_ptr<CHTLASTNode> child);
    std::vector<std::shared_ptr<CHTLASTNode>> getChildren() const;

protected:
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
    std::vector<std::shared_ptr<CHTLASTNode>> children;
};

/**
 * 自定义样式组节点
 */
class CustomStyleNode : public CustomNode {
public:
    CustomStyleNode(const std::string& name);
    
    std::string toString() const override;
    std::string generateCode() const override;
};

/**
 * 自定义元素节点
 */
class CustomElementNode : public CustomNode {
public:
    CustomElementNode(const std::string& name);
    
    std::string toString() const override;
    std::string generateCode() const override;
};

/**
 * 自定义变量组节点
 */
class CustomVarNode : public CustomNode {
public:
    CustomVarNode(const std::string& name);
    
    std::string toString() const override;
    std::string generateCode() const override;
};

/**
 * 原始嵌入节点
 */
class OriginNode : public CHTLASTNode {
public:
    OriginNode(NodeType type, const std::string& name, const std::string& content);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void setContent(const std::string& content);
    std::string getContent() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::string content;
};

/**
 * 导入节点
 */
class ImportNode : public CHTLASTNode {
public:
    ImportNode(NodeType type, const std::string& name, const std::string& path, const std::string& alias);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void setPath(const std::string& path);
    std::string getPath() const;
    
    void setAlias(const std::string& alias);
    std::string getAlias() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::string path;
    std::string alias;
};

/**
 * 配置节点
 */
class ConfigurationNode : public CHTLASTNode {
public:
    ConfigurationNode(const std::string& name = "");
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void addProperty(std::shared_ptr<CHTLASTNode> property);
    std::vector<std::shared_ptr<CHTLASTNode>> getProperties() const;
    
    void addNameGroup(std::shared_ptr<CHTLASTNode> nameGroup);
    std::shared_ptr<CHTLASTNode> getNameGroup() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
    std::shared_ptr<CHTLASTNode> nameGroup;
};

/**
 * 配置属性节点
 */
class ConfigPropertyNode : public CHTLASTNode {
public:
    ConfigPropertyNode(const std::string& name, std::shared_ptr<CHTLASTNode> value);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void setValue(std::shared_ptr<CHTLASTNode> value);
    std::shared_ptr<CHTLASTNode> getValue() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::shared_ptr<CHTLASTNode> value;
};

/**
 * 命名空间节点
 */
class NamespaceNode : public CHTLASTNode {
public:
    NamespaceNode(const std::string& name);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void addChild(std::shared_ptr<CHTLASTNode> child);
    std::vector<std::shared_ptr<CHTLASTNode>> getChildren() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> children;
};

/**
 * 信息节点
 */
class InfoNode : public CHTLASTNode {
public:
    InfoNode();
    
    void addProperty(std::shared_ptr<CHTLASTNode> property);
    std::vector<std::shared_ptr<CHTLASTNode>> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
};

/**
 * 导出节点
 */
class ExportNode : public CHTLASTNode {
public:
    ExportNode();
    
    void addExport(std::shared_ptr<CHTLASTNode> export_);
    std::vector<std::shared_ptr<CHTLASTNode>> getExports() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::vector<std::shared_ptr<CHTLASTNode>> exports;
};

/**
 * 字面量节点
 */
class LiteralNode : public CHTLASTNode {
public:
    LiteralNode(const std::string& value, const std::string& type = "string");
    
    void setValue(const std::string& value);
    std::string getValue() const;
    
    void setLiteralType(const std::string& type);
    std::string getLiteralType() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string value;
    std::string literalType;
};

/**
 * 标识符节点
 */
class IdentifierNode : public CHTLASTNode {
public:
    IdentifierNode(const std::string& name);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
};

/**
 * 表达式节点
 */
class ExpressionNode : public CHTLASTNode {
public:
    ExpressionNode(std::shared_ptr<CHTLASTNode> expression);
    
    void setExpression(std::shared_ptr<CHTLASTNode> expression);
    std::shared_ptr<CHTLASTNode> getExpression() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<CHTLASTNode> expression;
};

/**
 * 二元表达式节点
 */
class BinaryExpressionNode : public CHTLASTNode {
public:
    BinaryExpressionNode(std::shared_ptr<CHTLASTNode> left, const std::string& operator_, std::shared_ptr<CHTLASTNode> right);
    
    void setLeft(std::shared_ptr<CHTLASTNode> left);
    std::shared_ptr<CHTLASTNode> getLeft() const;
    
    void setOperator(const std::string& operator_);
    std::string getOperator() const;
    
    void setRight(std::shared_ptr<CHTLASTNode> right);
    std::shared_ptr<CHTLASTNode> getRight() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<CHTLASTNode> left;
    std::string operator_;
    std::shared_ptr<CHTLASTNode> right;
};

/**
 * 条件表达式节点
 */
class ConditionalExpressionNode : public CHTLASTNode {
public:
    ConditionalExpressionNode(std::shared_ptr<CHTLASTNode> condition, 
                             std::shared_ptr<CHTLASTNode> trueExpr, 
                             std::shared_ptr<CHTLASTNode> falseExpr);
    
    void setCondition(std::shared_ptr<CHTLASTNode> condition);
    std::shared_ptr<CHTLASTNode> getCondition() const;
    
    void setTrueExpression(std::shared_ptr<CHTLASTNode> trueExpr);
    std::shared_ptr<CHTLASTNode> getTrueExpression() const;
    
    void setFalseExpression(std::shared_ptr<CHTLASTNode> falseExpr);
    std::shared_ptr<CHTLASTNode> getFalseExpression() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<CHTLASTNode> condition;
    std::shared_ptr<CHTLASTNode> trueExpr;
    std::shared_ptr<CHTLASTNode> falseExpr;
};

/**
 * 属性引用节点
 */
class PropertyReferenceNode : public CHTLASTNode {
public:
    PropertyReferenceNode(const std::string& selector, const std::string& property);
    
    void setSelector(const std::string& selector);
    std::string getSelector() const;
    
    void setProperty(const std::string& property);
    std::string getProperty() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string selector;
    std::string property;
};

/**
 * 函数调用节点
 */
class FunctionCallNode : public CHTLASTNode {
public:
    FunctionCallNode(const std::string& functionName);
    
    void setFunctionName(const std::string& functionName);
    std::string getFunctionName() const;
    
    void addArgument(std::shared_ptr<CHTLASTNode> argument);
    std::vector<std::shared_ptr<CHTLASTNode>> getArguments() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string functionName;
    std::vector<std::shared_ptr<CHTLASTNode>> arguments;
};

/**
 * 使用语句节点
 */
class UseStatementNode : public CHTLASTNode {
public:
    UseStatementNode(const std::string& target);
    
    void setTarget(const std::string& target);
    std::string getTarget() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string target;
};

/**
 * 异常子句节点
 */
class ExceptClauseNode : public CHTLASTNode {
public:
    ExceptClauseNode();
    
    void addException(std::shared_ptr<CHTLASTNode> exception);
    std::vector<std::shared_ptr<CHTLASTNode>> getExceptions() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::vector<std::shared_ptr<CHTLASTNode>> exceptions;
};

} // namespace CHTL

#endif // CHTLASTNODE_H