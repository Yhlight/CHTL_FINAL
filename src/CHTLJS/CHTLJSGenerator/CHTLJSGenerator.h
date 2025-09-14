#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/RootNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include "../CHTLJSNode/VirNode.h"
#include "../CHTLJSNode/ValueNode.h"
#include <string>
#include <sstream>
#include <stdexcept>

namespace CHTLJS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator() = default;
    std::string generate(RootNode& root);

private:
    void visit(const CHTLJSNode* node);

    // Node-specific visit methods
    void visitRootNode(const RootNode* node);
    std::string visitEnhancedSelectorNode(const EnhancedSelectorNode* node);
    void visitListenNode(const ListenNode* node);
    void visitDelegateNode(const DelegateNode* node);
    void visitAnimateNode(const AnimateNode* node);
    void visitRouterNode(const RouterNode* node);
    void visitVirNode(const VirNode* node);
    std::string visitValueNode(const ValueNode* node);

    std::stringstream output_;
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H
