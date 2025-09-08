#ifndef TEMPLATE_USAGE_NODE_H
#define TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class TemplateType {
    Style,
    Element
};

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType templateType, const std::string& templateName);

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    TemplateType getTemplateType() const;
    const std::string& getTemplateName() const;

private:
    TemplateType m_templateType;
    std::string m_templateName;
};

} // namespace CHTL

#endif // TEMPLATE_USAGE_NODE_H
