#ifndef CHTL_OPERATORNODE_H
#define CHTL_OPERATORNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents an operator statement like 'delete', 'insert', etc.
class OperatorNode : public BaseNode {
public:
    explicit OperatorNode(std::string op) : op(std::move(op)) {}

    void accept(NodeVisitor& visitor) override;

private:
    std::string op; // The operator keyword (e.g., "delete")
    // Will have operands, targets, etc.
};

} // namespace CHTL

#endif // CHTL_OPERATORNODE_H
