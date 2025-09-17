#ifndef CHTL_CSS_PROPERTY_NODE_H
#define CHTL_CSS_PROPERTY_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CssPropertyNode : public BaseNode {
public:
    std::string key;
    std::shared_ptr<BaseNode> value;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<CssPropertyNode>();
        node->key = this->key;
        if (this->value) {
            node->value = this->value->clone();
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_CSS_PROPERTY_NODE_H
