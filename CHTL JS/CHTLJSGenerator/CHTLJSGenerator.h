#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include <string>
#include <memory>
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <vector>

// Forward declarations for CHTL JS Nodes to avoid circular dependencies
namespace CHTL_JS {
    class CHTLJSBaseNode;
    class ListenNode;
    class EnhancedSelectorNode;
    class DelegateNode; // Forward-declare DelegateNode
    class RawJSNode;
    class EventHandlerNode;
    class AnimateNode;
}

namespace CHTL_JS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();

    std::string generate(const std::vector<std::unique_ptr<CHTLJSBaseNode>>& nodes);

private:
    std::string m_output;
    void visit(CHTLJSBaseNode* node);
};

} // namespace CHTL_JS

#endif // CHTL_JS_GENERATOR_H
