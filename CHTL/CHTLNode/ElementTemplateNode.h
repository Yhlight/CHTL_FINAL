#ifndef CHTL_ELEMENT_TEMPLATE_NODE_H
#define CHTL_ELEMENT_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class ElementTemplateNode : public BaseNode {
public:
    std::string name;
    // The CHTL nodes that make up the template's body
    std::vector<std::shared_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_ELEMENT_TEMPLATE_NODE_H
