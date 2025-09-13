#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>

namespace CHTLJS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(const CHTLJSNode& root);

private:
    void visit(const CHTLJSNode* node);
    // Add visit methods for each node type, e.g.:
    // void visitEnhancedSelector(const EnhancedSelectorNode* node);

    std::string output_;
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H
