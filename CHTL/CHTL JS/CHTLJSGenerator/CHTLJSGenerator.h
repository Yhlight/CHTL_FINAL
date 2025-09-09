#pragma once

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSNode/CHTLJSNodes.h"
#include <string>
#include <sstream>

namespace CHTL {
namespace CHTL_JS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(CHTLJSProgram* program);

private:
    void visit(CHTLJSNode* node);
    void visit(CHTLJSProgram* node);
    void visit(EnhancedSelectorNode* node);
    void visit(ListenNode* node);
    void visit(RawJavaScriptNode* node);

    std::stringstream m_out;
};

} // namespace CHTL_JS
} // namespace CHTL
