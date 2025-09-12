#ifndef CHTL_EXPRNODE_H
#define CHTL_EXPRNODE_H

namespace CHTL {

// Forward declare the visitor for expression nodes
class ExprVisitor;

// Base class for all expression nodes.
// Note: This is a separate hierarchy from BaseNode for document structure.
class ExprNode {
public:
    virtual ~ExprNode() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

} // namespace CHTL

#endif // CHTL_EXPRNODE_H
