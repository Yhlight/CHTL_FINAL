#ifndef TEMPLATE_NODE_H
#define TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * 模板节点
 */
class TemplateNode : public BaseNode {
public:
    enum class TemplateType {
        STYLE,          // 样式组模板
        ELEMENT,        // 元素模板
        VAR             // 变量组模板
    };

    TemplateNode(TemplateType type, const std::string& name);
    virtual ~TemplateNode() = default;
    
    // 获取/设置模板类型
    TemplateType getTemplateType() const { return m_templateType; }
    void setTemplateType(TemplateType type) { m_templateType = type; }
    
    // 获取/设置模板名称
    const std::string& getTemplateName() const { return m_templateName; }
    void setTemplateName(const std::string& name) { m_templateName = name; }
    
    // 检查是否为样式组模板
    bool isStyleTemplate() const { return m_templateType == TemplateType::STYLE; }
    
    // 检查是否为元素模板
    bool isElementTemplate() const { return m_templateType == TemplateType::ELEMENT; }
    
    // 检查是否为变量组模板
    bool isVarTemplate() const { return m_templateType == TemplateType::VAR; }
    
    // 克隆节点
    std::shared_ptr<BaseNode> clone() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
    // 接受访问者
    void accept(NodeVisitor& visitor) override;

private:
    TemplateType m_templateType;
    std::string m_templateName;
};

} // namespace CHTL

#endif // TEMPLATE_NODE_H