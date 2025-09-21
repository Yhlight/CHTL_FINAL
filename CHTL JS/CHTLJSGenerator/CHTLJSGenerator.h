#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include <string>
#include <memory>
#include "../CHTLJSNode/CHTLJSBaseNode.h"
// Forward declarations for CHTL JS Nodes to avoid circular dependencies
// We will need to include the actual node headers in the .cpp file.
namespace CHTL_JS {
    class CHTLJSBaseNode;
    class ListenNode;
    class EnhancedSelectorNode;
    // ... other node types
}

namespace CHTL_JS {

    class CHTLJSGenerator {
    public:
        CHTLJSGenerator();

        std::string generate(CHTLJSBaseNode* root);

        // Visitor methods for each node type
        // void visit(ListenNode* node);
        // void visit(EnhancedSelectorNode* node);
        // ...

    private:
        std::string m_output;

        void visit(CHTLJSBaseNode* node);
    };

} // namespace CHTL_JS

#endif // CHTL_JS_GENERATOR_H
