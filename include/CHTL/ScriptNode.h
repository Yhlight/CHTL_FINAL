#pragma once

#include "BaseNode.h"

namespace CHTL {

// 脚本节点类
class ScriptNode : public BaseNode {
private:
    String content_;
    bool isGlobal_;
    bool isLocal_;
    
public:
    ScriptNode(const SourceLocation& loc = {})
        : BaseNode(NodeType::Script, loc), isGlobal_(false), isLocal_(false) {}
    
    // 内容管理
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    // 脚本类型
    bool isGlobal() const { return isGlobal_; }
    void setGlobal(bool global) { isGlobal_ = global; }
    
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 工厂方法
    static ScriptNodePtr create(const SourceLocation& loc = {});
    static ScriptNodePtr createGlobal(const SourceLocation& loc = {});
    static ScriptNodePtr createLocal(const SourceLocation& loc = {});
    static ScriptNodePtr createWithContent(const String& content, const SourceLocation& loc = {});
};

} // namespace CHTL