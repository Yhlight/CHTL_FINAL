#pragma once

#include "BaseNode.h"

namespace CHTL {

// 自定义节点类
class CustomNode : public BaseNode {
private:
    String customType_;
    String customName_;
    StringMap parameters_;
    
public:
    CustomNode(const String& type, const String& name, const SourceLocation& loc = {})
        : BaseNode(NodeType::Custom, loc), customType_(type), customName_(name) {}
    
    // 自定义类型和名称
    const String& getCustomType() const { return customType_; }
    void setCustomType(const String& type) { customType_ = type; }
    
    const String& getCustomName() const { return customName_; }
    void setCustomName(const String& name) { customName_ = name; }
    
    // 参数管理
    const StringMap& getParameters() const { return parameters_; }
    void setParameter(const String& name, const String& value);
    String getParameter(const String& name) const;
    bool hasParameter(const String& name) const;
    void removeParameter(const String& name);
    void clearParameters();
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 工厂方法
    static CustomNodePtr create(const String& type, const String& name, const SourceLocation& loc = {});
};

} // namespace CHTL