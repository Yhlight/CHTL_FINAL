#pragma once

#include "BaseNode.h"

namespace CHTL {

// 命名空间节点类
class NamespaceNode : public BaseNode {
private:
    String namespaceName_;
    
public:
    NamespaceNode(const String& name, const SourceLocation& loc = {})
        : BaseNode(NodeType::Namespace, loc), namespaceName_(name) {}
    
    // 命名空间名称
    const String& getNamespaceName() const { return namespaceName_; }
    void setNamespaceName(const String& name) { namespaceName_ = name; }
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 工厂方法
    static NamespaceNodePtr create(const String& name, const SourceLocation& loc = {});
};

} // namespace CHTL