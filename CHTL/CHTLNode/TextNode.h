#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text);
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::string text;
};

} // namespace CHTL

#endif // TEXT_NODE_H
