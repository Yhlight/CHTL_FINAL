#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/RootNode.h"
#include <string>

namespace CHTLJS {

// Forward declarations
class PlaceholderNode;
class EnhancedSelectorNode;
class ListenNode;
class DelegateNode;
class AnimateNode;
class ValueNode;

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(const RootNode& root);

private:
    void visit(const CHTLJSNode* node);
    void visitRootNode(const RootNode* node);
    void visitPlaceholderNode(const PlaceholderNode* node);
    void visitEnhancedSelector(const EnhancedSelectorNode* node);
    void visitListenNode(const ListenNode* node);
    void visitDelegateNode(const DelegateNode* node);
    void visitAnimateNode(const AnimateNode* node);
    void visitValueNode(const ValueNode* node);

    std::string output_;
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H
