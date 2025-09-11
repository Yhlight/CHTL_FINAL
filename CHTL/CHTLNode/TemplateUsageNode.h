#ifndef CHTL_TEMPLATE_USAGE_NODE_H
#define CHTL_TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include "TemplateNode.h" // For TemplateType enum
#include <string>
#include <vector>
#include <memory>

// Represents the usage of a template, e.g., `@Style DefaultText;`
class TemplateUsageNode : public BaseNode {
public:
    TemplateType type;
    std::string name;
    // This will hold specialization nodes later (e.g., for `delete` or overrides)
    std::vector<std::unique_ptr<BaseNode>> specializations;

    TemplateUsageNode(TemplateType t, const std::string& n) : type(t), name(n) {}
};

#endif // CHTL_TEMPLATE_USAGE_NODE_H
