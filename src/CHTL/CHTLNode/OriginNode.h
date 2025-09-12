#ifndef CHTL_ORIGINNODE_H
#define CHTL_ORIGINNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents an [Origin] block
class OriginNode : public BaseNode {
public:
    explicit OriginNode(std::string type, std::string content)
        : type(std::move(type)), content(std::move(content)) {}

    void accept(NodeVisitor& visitor) override;

    const std::string& getType() const { return type; }
    const std::string& getContent() const { return content; }

private:
    std::string type;    // e.g., @Html, @Style
    std::string content; // The raw content
};

} // namespace CHTL

#endif // CHTL_ORIGINNODE_H
