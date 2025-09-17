#pragma once

#include "BaseNode.h"

namespace CHTL {

// 模板节点类
class TemplateNode : public BaseNode {
private:
    String templateType_;
    String templateName_;
    StringMap parameters_;
    
public:
    TemplateNode(const String& type, const String& name, const SourceLocation& loc = {})
        : BaseNode(NodeType::Template, loc), templateType_(type), templateName_(name) {}
    
    // 模板类型和名称
    const String& getTemplateType() const { return templateType_; }
    void setTemplateType(const String& type) { templateType_ = type; }
    
    const String& getTemplateName() const { return templateName_; }
    void setTemplateName(const String& name) { templateName_ = name; }
    
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
    static TemplateNodePtr create(const String& type, const String& name, const SourceLocation& loc = {});
};

} // namespace CHTL