#ifndef ORIGIN_NODE_H
#define ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class OriginType {
    HTML,
    STYLE,
    JAVASCRIPT
};

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& content, OriginType type, const std::string& name = "")
        : content(content), type(type), name(name) {}
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::string content;
    OriginType type;
    std::string name;
};

} // namespace CHTL

#endif // ORIGIN_NODE_H
