#ifndef CHTL_STYLENODE_H
#define CHTL_STYLENODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a style {} block
class StyleNode : public BaseNode {
public:
    explicit StyleNode() = default;

    void accept(NodeVisitor& visitor) override;

private:
    // Will contain style properties, selectors, etc.
};

} // namespace CHTL

#endif // CHTL_STYLENODE_H
