#ifndef CHTLJS_VISITOR_H
#define CHTLJS_VISITOR_H

namespace CHTLJS {

// Forward declare all node types
class CHTLJS_SelectorNode;
class CHTLJS_ListenNode;
class CHTLJS_RawJSNode;

class CHTLJS_Visitor {
public:
    virtual ~CHTLJS_Visitor() = default;
    virtual void visit(CHTLJS_SelectorNode& node) = 0;
    virtual void visit(CHTLJS_ListenNode& node) = 0;
    virtual void visit(CHTLJS_RawJSNode& node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_VISITOR_H
