#ifndef CHTL_TEXTNODE_H
#define CHTL_TEXTNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(std::string text) : text(std::move(text)) {}

    void accept(NodeVisitor& visitor) override;

    const std::string& getText() const { return text; }

private:
    std::string text;
};

} // namespace CHTL

#endif // CHTL_TEXTNODE_H
