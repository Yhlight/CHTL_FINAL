#ifndef CHTL_ATTRIBUTE_NODE_H
#define CHTL_ATTRIBUTE_NODE_H

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    std::string key;
    std::shared_ptr<BaseNode> value;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<AttributeNode>();
        node->key = this->key;
        if (this->value) {
            node->value = this->value->clone();
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_NODE_H
