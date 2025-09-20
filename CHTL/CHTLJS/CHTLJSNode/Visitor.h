#ifndef CHTLJS_VISITOR_H
#define CHTLJS_VISITOR_H

namespace CHTL_JS {

// Forward declarations of node types
class CHTLJS_BaseNode;

class Visitor {
public:
    virtual ~Visitor() = default;

    // Add visit methods for each concrete node type here
    // virtual void visit(SomeNode& node) = 0;
};

} // namespace CHTL_JS

#endif // CHTLJS_VISITOR_H
