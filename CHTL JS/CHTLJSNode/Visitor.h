#pragma once

namespace CHTL_JS {

// Forward declarations
class AnimateNode;
class DelegateNode;
class EnhancedSelectorNode;
class EventHandlerNode;
class ListenNode;
class RawJSNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(AnimateNode& node) = 0;
    virtual void visit(DelegateNode& node) = 0;
    virtual void visit(EnhancedSelectorNode& node) = 0;
    virtual void visit(EventHandlerNode& node) = 0;
    virtual void visit(ListenNode& node) = 0;
    virtual void visit(RawJSNode& node) = 0;
};

} // namespace CHTL_JS
