#ifndef CHTL_ELEMENT_TEMPLATE_USAGE_NODE_H
#define CHTL_ELEMENT_TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include <string>

class ElementTemplateUsageNode : public BaseNode {
public:
    std::string name;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_ELEMENT_TEMPLATE_USAGE_NODE_H
