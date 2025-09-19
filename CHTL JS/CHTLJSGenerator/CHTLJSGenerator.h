#pragma once

#include "../CHTLJSNode/Visitor.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <vector>
#include <string>
#include <sstream>
#include <memory>

namespace CHTL_JS {

class CHTLJSGenerator : public Visitor {
public:
    CHTLJSGenerator();
    std::string generate(const std::vector<std::unique_ptr<CHTLJSBaseNode>>& nodes);

    void visit(AnimateNode& node) override;
    void visit(DelegateNode& node) override;
    void visit(EnhancedSelectorNode& node) override;
    void visit(EventHandlerNode& node) override;
    void visit(ListenNode& node) override;
    void visit(RawJSNode& node) override;

private:
    std::stringstream js_output;
};

} // namespace CHTL_JS
