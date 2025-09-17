#ifndef CHTL_VAR_DECLARATION_NODE_H
#define CHTL_VAR_DECLARATION_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class VarDeclarationNode : public BaseNode {
public:
    std::string name;
    std::string value;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<VarDeclarationNode>();
        node->name = this->name;
        node->value = this->value;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_VAR_DECLARATION_NODE_H
