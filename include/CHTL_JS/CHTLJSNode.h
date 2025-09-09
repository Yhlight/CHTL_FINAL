#pragma once

#include "CHTLJSCommon.h"
#include <memory>
#include <vector>

namespace CHTL_JS {

// 基础节点类
class CHTLJSNode {
public:
    CHTLJSNode(CHTLJSNodeType type, const Position& pos = Position());
    virtual ~CHTLJSNode() = default;
    
    // 获取节点类型
    CHTLJSNodeType getType() const { return type_; }
    
    // 获取位置信息
    const Position& getPosition() const { return position_; }
    void setPosition(const Position& pos) { position_ = pos; }
    
    // 获取/设置父节点
    std::shared_ptr<CHTLJSNode> getParent() const { return parent_.lock(); }
    void setParent(std::shared_ptr<CHTLJSNode> parent) { parent_ = parent; }
    
    // 子节点管理
    const std::vector<std::shared_ptr<CHTLJSNode>>& getChildren() const { return children_; }
    void addChild(std::shared_ptr<CHTLJSNode> child);
    void removeChild(std::shared_ptr<CHTLJSNode> child);
    void clearChildren();
    
    // 文本内容
    const String& getText() const { return text_; }
    void setText(const String& text) { text_ = text; }
    
    // 克隆节点
    virtual std::shared_ptr<CHTLJSNode> clone() const;
    
    // 访问者模式
    virtual void accept(class CHTLJSVisitor& visitor) = 0;

protected:
    CHTLJSNodeType type_;
    Position position_;
    std::weak_ptr<CHTLJSNode> parent_;
    std::vector<std::shared_ptr<CHTLJSNode>> children_;
    String text_;
};

// 标识符节点
class IdentifierNode : public CHTLJSNode {
public:
    IdentifierNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
};

// 字面量节点
class LiteralNode : public CHTLJSNode {
public:
    LiteralNode(const LiteralValue& value, const Position& pos = Position());
    
    const LiteralValue& getValue() const { return value_; }
    void setValue(const LiteralValue& value) { value_ = value; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    LiteralValue value_;
};

// 表达式节点
class ExpressionNode : public CHTLJSNode {
public:
    ExpressionNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// 语句节点
class StatementNode : public CHTLJSNode {
public:
    StatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// 块节点
class BlockNode : public CHTLJSNode {
public:
    BlockNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// 函数节点
class FunctionNode : public CHTLJSNode {
public:
    FunctionNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    const StringList& getParameters() const { return parameters_; }
    void addParameter(const String& param) { parameters_.push_back(param); }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
    StringList parameters_;
};

// 函数调用节点
class FunctionCallNode : public CHTLJSNode {
public:
    FunctionCallNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    const std::vector<std::shared_ptr<CHTLJSNode>>& getArguments() const { return arguments_; }
    void addArgument(std::shared_ptr<CHTLJSNode> arg) { arguments_.push_back(arg); }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
    std::vector<std::shared_ptr<CHTLJSNode>> arguments_;
};

// 箭头函数节点
class ArrowFunctionNode : public CHTLJSNode {
public:
    ArrowFunctionNode(const Position& pos = Position());
    
    const StringList& getParameters() const { return parameters_; }
    void addParameter(const String& param) { parameters_.push_back(param); }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    StringList parameters_;
};

// 对象节点
class ObjectNode : public CHTLJSNode {
public:
    ObjectNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// 对象属性节点
class ObjectPropertyNode : public CHTLJSNode {
public:
    ObjectPropertyNode(const String& key, const Position& pos = Position());
    
    const String& getKey() const { return key_; }
    void setKey(const String& key) { key_ = key; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String key_;
};

// 数组节点
class ArrayNode : public CHTLJSNode {
public:
    ArrayNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// 数组元素节点
class ArrayElementNode : public CHTLJSNode {
public:
    ArrayElementNode(int index, const Position& pos = Position());
    
    int getIndex() const { return index_; }
    void setIndex(int index) { index_ = index; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    int index_;
};

// 控制流节点基类
class ControlFlowNode : public CHTLJSNode {
public:
    ControlFlowNode(CHTLJSNodeType type, const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override = 0;
};

// if 语句节点
class IfStatementNode : public ControlFlowNode {
public:
    IfStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// for 循环节点
class ForStatementNode : public ControlFlowNode {
public:
    ForStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// while 循环节点
class WhileStatementNode : public ControlFlowNode {
public:
    WhileStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// switch 语句节点
class SwitchStatementNode : public ControlFlowNode {
public:
    SwitchStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// case 语句节点
class CaseStatementNode : public ControlFlowNode {
public:
    CaseStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// break 语句节点
class BreakStatementNode : public ControlFlowNode {
public:
    BreakStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// continue 语句节点
class ContinueStatementNode : public ControlFlowNode {
public:
    ContinueStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// return 语句节点
class ReturnStatementNode : public ControlFlowNode {
public:
    ReturnStatementNode(const Position& pos = Position());
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;
};

// 变量声明节点
class VariableDeclarationNode : public CHTLJSNode {
public:
    VariableDeclarationNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
};

// 变量赋值节点
class VariableAssignmentNode : public CHTLJSNode {
public:
    VariableAssignmentNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
};

// 操作符节点
class OperatorNode : public CHTLJSNode {
public:
    OperatorNode(OperatorType type, const Position& pos = Position());
    
    OperatorType getOperatorType() const { return operatorType_; }
    void setOperatorType(OperatorType type) { operatorType_ = type; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    OperatorType operatorType_;
};

// 虚对象节点
class VirtualObjectNode : public CHTLJSNode {
public:
    VirtualObjectNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    const VirtualObjectConfig& getConfig() const { return config_; }
    void setConfig(const VirtualObjectConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
    VirtualObjectConfig config_;
};

// 监听器节点
class ListenNode : public CHTLJSNode {
public:
    ListenNode(const Position& pos = Position());
    
    const ListenerConfig& getConfig() const { return config_; }
    void setConfig(const ListenerConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    ListenerConfig config_;
};

// 事件委托节点
class DelegateNode : public CHTLJSNode {
public:
    DelegateNode(const Position& pos = Position());
    
    const DelegateConfig& getConfig() const { return config_; }
    void setConfig(const DelegateConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    DelegateConfig config_;
};

// 动画节点
class AnimateNode : public CHTLJSNode {
public:
    AnimateNode(const Position& pos = Position());
    
    const AnimationConfig& getConfig() const { return config_; }
    void setConfig(const AnimationConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    AnimationConfig config_;
};

// 路由节点
class RouterNode : public CHTLJSNode {
public:
    RouterNode(const Position& pos = Position());
    
    const RouteConfig& getConfig() const { return config_; }
    void setConfig(const RouteConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    RouteConfig config_;
};

// 文件加载器节点
class FileLoaderNode : public CHTLJSNode {
public:
    FileLoaderNode(const Position& pos = Position());
    
    const FileLoaderConfig& getConfig() const { return config_; }
    void setConfig(const FileLoaderConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    FileLoaderConfig config_;
};

// 工具函数节点
class UtilNode : public CHTLJSNode {
public:
    UtilNode(const Position& pos = Position());
    
    const UtilConfig& getConfig() const { return config_; }
    void setConfig(const UtilConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    UtilConfig config_;
};

// 打印节点
class PrintNode : public CHTLJSNode {
public:
    PrintNode(const Position& pos = Position());
    
    const PrintConfig& getConfig() const { return config_; }
    void setConfig(const PrintConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    PrintConfig config_;
};

// 永不离开节点
class INeverAwayNode : public CHTLJSNode {
public:
    INeverAwayNode(const String& name, const Position& pos = Position());
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    const INeverAwayConfig& getConfig() const { return config_; }
    void setConfig(const INeverAwayConfig& config) { config_ = config; }
    
    void accept(class CHTLJSVisitor& visitor) override;
    std::shared_ptr<CHTLJSNode> clone() const override;

private:
    String name_;
    INeverAwayConfig config_;
};

} // namespace CHTL_JS