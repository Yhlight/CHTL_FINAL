#pragma once

#include "Common.h"
#include "Token.h"
#include <memory>
#include <vector>

namespace CHTL {

// 基础节点类
class BaseNode : public std::enable_shared_from_this<BaseNode> {
protected:
    NodeType nodeType_;
    SourceLocation location_;
    NodePtr parent_;
    std::vector<NodePtr> children_;
    
public:
    BaseNode(NodeType type, const SourceLocation& loc = {})
        : nodeType_(type), location_(loc) {}
    
    virtual ~BaseNode() = default;
    
    // 基本访问器
    NodeType getNodeType() const { return nodeType_; }
    const SourceLocation& getLocation() const { return location_; }
    void setLocation(const SourceLocation& loc) { location_ = loc; }
    
    // 父节点管理
    NodePtr getParent() const { return parent_; }
    void setParent(const NodePtr& parent) { parent_ = parent; }
    
    // 子节点管理
    const std::vector<NodePtr>& getChildren() const { return children_; }
    void addChild(const NodePtr& child);
    void removeChild(const NodePtr& child);
    void insertChild(size_t index, const NodePtr& child);
    void clearChildren();
    
    // 查找子节点
    NodePtr findChild(NodeType type) const;
    std::vector<NodePtr> findChildren(NodeType type) const;
    NodePtr findChildByType(NodeType type, size_t index = 0) const;
    
    // 遍历
    template<typename Func>
    void forEachChild(Func func) const {
        for (const auto& child : children_) {
            func(child);
        }
    }
    
    template<typename Func>
    void forEachChild(Func func) {
        for (auto& child : children_) {
            func(child);
        }
    }
    
    // 虚拟方法
    virtual String toString() const = 0;
    virtual String toHTML() const = 0;
    virtual bool isValid() const { return true; }
    virtual void accept(class NodeVisitor& visitor) = 0;
    
    // 类型检查
    bool isElement() const { return nodeType_ == NodeType::Element; }
    bool isText() const { return nodeType_ == NodeType::Text; }
    bool isStyle() const { return nodeType_ == NodeType::Style; }
    bool isScript() const { return nodeType_ == NodeType::Script; }
    bool isTemplate() const { return nodeType_ == NodeType::Template; }
    bool isCustom() const { return nodeType_ == NodeType::Custom; }
    bool isImport() const { return nodeType_ == NodeType::Import; }
    bool isNamespace() const { return nodeType_ == NodeType::Namespace; }
    bool isConfiguration() const { return nodeType_ == NodeType::Configuration; }
    bool isOrigin() const { return nodeType_ == NodeType::Origin; }
    bool isOperator() const { return nodeType_ == NodeType::Operator; }
    bool isCHTLJSFunction() const { return nodeType_ == NodeType::CHTLJSFunction; }
    bool isVirtualObject() const { return nodeType_ == NodeType::VirtualObject; }
    bool isEventBinding() const { return nodeType_ == NodeType::EventBinding; }
    bool isAnimation() const { return nodeType_ == NodeType::Animation; }
    bool isRouter() const { return nodeType_ == NodeType::Router; }
};

// 访问者模式
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visit(BaseNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(ConfigurationNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(OperatorNode& node) = 0;
};

// 默认访问者实现
class DefaultNodeVisitor : public NodeVisitor {
public:
    void visit(BaseNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(TemplateNode& node) override;
    void visit(CustomNode& node) override;
    void visit(ImportNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(ConfigurationNode& node) override;
    void visit(OriginNode& node) override;
    void visit(OperatorNode& node) override;
};

} // namespace CHTL