#ifndef TEMPLATE_USAGE_NODE_H
#define TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    std::string template_name;
    std::string from_namespace;

    explicit TemplateUsageNode(const std::string& name, const std::string& from = "");

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // TEMPLATE_USAGE_NODE_H
