#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<TextNode>();
        node->content = this->content;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
