#ifndef ORIGIN_NODE_H
#define ORIGIN_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>

namespace CHTL {

enum class OriginType {
    HTML,
    STYLE,
    JAVASCRIPT
};

// Represents a block of raw, un-parsed code to be embedded directly
// into the output.
class OriginNode : public BaseNode {
public:
    OriginNode(OriginType type, const std::string& content)
        : type(type), content(content) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    OriginType type;
    std::string content;
};

} // namespace CHTL

#endif // ORIGIN_NODE_H
