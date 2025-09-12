#ifndef CHTL_SCRIPTNODE_H
#define CHTL_SCRIPTNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a script {} block
class ScriptNode : public BaseNode {
public:
    explicit ScriptNode() = default;

    void accept(NodeVisitor& visitor) override;

private:
    // Will contain script content
};

} // namespace CHTL

#endif // CHTL_SCRIPTNODE_H
