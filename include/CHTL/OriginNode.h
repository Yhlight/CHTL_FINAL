#pragma once

#include "BaseNode.h"

namespace CHTL {

// 原始嵌入节点类
class OriginNode : public BaseNode {
private:
    String originType_;
    String originName_;
    String content_;
    
public:
    OriginNode(const String& type, const String& name, const SourceLocation& loc = {})
        : BaseNode(NodeType::Origin, loc), originType_(type), originName_(name) {}
    
    // 原始类型和名称
    const String& getOriginType() const { return originType_; }
    void setOriginType(const String& type) { originType_ = type; }
    
    const String& getOriginName() const { return originName_; }
    void setOriginName(const String& name) { originName_ = name; }
    
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 工厂方法
    static OriginNodePtr create(const String& type, const String& name, const SourceLocation& loc = {});
};

} // namespace CHTL