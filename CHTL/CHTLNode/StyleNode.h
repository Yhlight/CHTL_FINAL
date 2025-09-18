#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "Visitor.h"
#include <vector>

namespace CHTL {

// Represents a style { ... } block within an element.
class StyleNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;

    // A style block contains a list of CSS properties.
    // We can reuse AttributeNode to represent a property (e.g., key="width", value="100px").
    std::vector<AttributeNode> properties;
};

} // namespace CHTL

#endif // STYLE_NODE_H
