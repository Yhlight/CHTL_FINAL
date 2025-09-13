#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>

namespace CHTLJS {

// Forward declarations
class EnhancedSelectorNode;
class ListenNode;

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(const CHTLJSNode& root);

private:
    void visit(const CHTLJSNode* node);
    void visitEnhancedSelector(const EnhancedSelectorNode* node);
    void visitListenNode(const ListenNode* node);

    std::string output_;
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H
