#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTLJS {

/**
 * AST节点基类
 */
class ASTNode {
public:
    enum class NodeType {
        PROGRAM,
        EXPRESSION,
        STATEMENT,
        FUNCTION_CALL,
        OBJECT_LITERAL,
        ARRAY_LITERAL,
        IDENTIFIER,
        LITERAL,
        BINARY_OPERATION,
        UNARY_OPERATION,
        VIRTUAL_OBJECT,
        LISTEN_EXPRESSION,
        ANIMATE_EXPRESSION,
        ROUTER_EXPRESSION,
        SCRIPT_LOADER_EXPRESSION,
        DELEGATE_EXPRESSION,
        INEVERAWAY_EXPRESSION,
        UTIL_EXPRESSION,
        SELECTOR_EXPRESSION,
        EVENT_BINDING,
        PROPERTY_ACCESS
    };

    ASTNode(NodeType type);
    virtual ~ASTNode() = default;

    NodeType getType() const;
    virtual std::string toString() const = 0;
    virtual std::string generateCode() const = 0;

protected:
    NodeType type;
};

/**
 * 程序节点
 */
class ProgramNode : public ASTNode {
public:
    ProgramNode();
    void addStatement(std::shared_ptr<ASTNode> statement);
    std::vector<std::shared_ptr<ASTNode>> getStatements() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::vector<std::shared_ptr<ASTNode>> statements;
};

/**
 * 标识符节点
 */
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const std::string& name);
    std::string getName() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
};

/**
 * 字面量节点
 */
class LiteralNode : public ASTNode {
public:
    LiteralNode(const std::string& value, const std::string& type);
    std::string getValue() const;
    std::string getLiteralType() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string value;
    std::string literalType;
};

/**
 * 对象字面量节点
 */
class ObjectLiteralNode : public ASTNode {
public:
    ObjectLiteralNode();
    void addProperty(const std::string& key, std::shared_ptr<ASTNode> value);
    std::map<std::string, std::shared_ptr<ASTNode>> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::map<std::string, std::shared_ptr<ASTNode>> properties;
};

/**
 * 函数调用节点
 */
class FunctionCallNode : public ASTNode {
public:
    FunctionCallNode(const std::string& functionName);
    void addArgument(std::shared_ptr<ASTNode> argument);
    std::string getFunctionName() const;
    std::vector<std::shared_ptr<ASTNode>> getArguments() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string functionName;
    std::vector<std::shared_ptr<ASTNode>> arguments;
};

/**
 * 虚对象节点
 */
class VirtualObjectNode : public ASTNode {
public:
    VirtualObjectNode(const std::string& name, std::shared_ptr<ObjectLiteralNode> definition);
    std::string getName() const;
    std::shared_ptr<ObjectLiteralNode> getDefinition() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string name;
    std::shared_ptr<ObjectLiteralNode> definition;
};

/**
 * 监听器节点
 */
class ListenNode : public ASTNode {
public:
    ListenNode(std::shared_ptr<ObjectLiteralNode> events);
    std::shared_ptr<ObjectLiteralNode> getEvents() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ObjectLiteralNode> events;
};

/**
 * 动画节点
 */
class AnimateNode : public ASTNode {
public:
    AnimateNode(std::shared_ptr<ObjectLiteralNode> properties);
    std::shared_ptr<ObjectLiteralNode> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ObjectLiteralNode> properties;
};

/**
 * 路由节点
 */
class RouterNode : public ASTNode {
public:
    RouterNode(std::shared_ptr<ObjectLiteralNode> properties);
    std::shared_ptr<ObjectLiteralNode> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ObjectLiteralNode> properties;
};

/**
 * 脚本加载器节点
 */
class ScriptLoaderNode : public ASTNode {
public:
    ScriptLoaderNode(std::shared_ptr<ObjectLiteralNode> properties);
    std::shared_ptr<ObjectLiteralNode> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ObjectLiteralNode> properties;
};

/**
 * 事件委托节点
 */
class DelegateNode : public ASTNode {
public:
    DelegateNode(std::shared_ptr<ObjectLiteralNode> properties);
    std::shared_ptr<ObjectLiteralNode> getProperties() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ObjectLiteralNode> properties;
};

/**
 * iNeverAway节点
 */
class INeverAwayNode : public ASTNode {
public:
    INeverAwayNode(std::shared_ptr<ObjectLiteralNode> functions);
    std::shared_ptr<ObjectLiteralNode> getFunctions() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ObjectLiteralNode> functions;
};

/**
 * util表达式节点
 */
class UtilExpressionNode : public ASTNode {
public:
    UtilExpressionNode(std::shared_ptr<ASTNode> condition, 
                      std::shared_ptr<ASTNode> changeBlock,
                      std::shared_ptr<ASTNode> thenBlock);
    std::shared_ptr<ASTNode> getCondition() const;
    std::shared_ptr<ASTNode> getChangeBlock() const;
    std::shared_ptr<ASTNode> getThenBlock() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> changeBlock;
    std::shared_ptr<ASTNode> thenBlock;
};

/**
 * 选择器节点
 */
class SelectorNode : public ASTNode {
public:
    SelectorNode(const std::string& selector);
    std::string getSelector() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::string selector;
};

/**
 * 事件绑定节点
 */
class EventBindingNode : public ASTNode {
public:
    EventBindingNode(std::shared_ptr<SelectorNode> selector, 
                    const std::string& eventType,
                    std::shared_ptr<ASTNode> handler);
    std::shared_ptr<SelectorNode> getSelector() const;
    std::string getEventType() const;
    std::shared_ptr<ASTNode> getHandler() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<SelectorNode> selector;
    std::string eventType;
    std::shared_ptr<ASTNode> handler;
};

/**
 * 属性访问节点
 */
class PropertyAccessNode : public ASTNode {
public:
    PropertyAccessNode(std::shared_ptr<ASTNode> object, const std::string& property);
    std::shared_ptr<ASTNode> getObject() const;
    std::string getProperty() const;
    
    std::string toString() const override;
    std::string generateCode() const override;

private:
    std::shared_ptr<ASTNode> object;
    std::string property;
};

} // namespace CHTLJS

#endif // ASTNODE_H