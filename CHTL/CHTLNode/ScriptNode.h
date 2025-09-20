#ifndef SCRIPT_NODE_H
#define SCRIPT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>

namespace CHTL {

// Represents a script { ... } block within an element.
class ScriptNode : public BaseNode {
public:
    explicit ScriptNode(const std::string& content) : content(content) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::string content;
};

} // namespace CHTL

#endif // SCRIPT_NODE_H
