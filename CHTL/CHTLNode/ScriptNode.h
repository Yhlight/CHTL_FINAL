#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>

// Represents a script { ... } block in the AST.
// It holds the placeholder for the script content, which will be resolved by the generator.
class ScriptNode : public BaseNode {
public:
    std::string placeholder;

    explicit ScriptNode(const std::string& ph) : placeholder(ph) {}
};

#endif // CHTL_SCRIPT_NODE_H
