#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "CHTLJSNode/CHTLJSBaseNode.h"
#include "CHTLJSNode/AnimateNode.h"
#include "CHTLJSNode/ListenNode.h"
#include "CHTLJSNode/DelegateNode.h"
#include "CHTLJSNode/EventHandlerNode.h"
#include "CHTLJSNode/RawJSNode.h"
#include "CHTLJSNode/EnhancedSelectorNode.h"

#include <string>
#include <sstream>
#include <vector>

#include "CHTLJSCompilationResult.h"

namespace CHTL_JS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    CHTLJSCompilationResult generate(const std::vector<std::unique_ptr<CHTLJSBaseNode>>& nodes);

private:
    void visit(CHTLJSBaseNode* node, CHTLJSCompilationResult& result);
    void visitAnimateNode(AnimateNode* node);
    void visitListenNode(ListenNode* node);
    void visitDelegateNode(DelegateNode* node);
    void visitEventHandlerNode(EventHandlerNode* node);
    void visitRawJSNode(RawJSNode* node);
    void visitEnhancedSelectorNode(EnhancedSelectorNode* node);

    std::stringstream js_output;
};

} // namespace CHTL_JS

#endif // CHTL_JS_GENERATOR_H
