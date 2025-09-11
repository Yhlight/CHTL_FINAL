#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleSelectorNode.h"

namespace CHTL {

class CHTLSemanticAnalyzer {
public:
    CHTLSemanticAnalyzer() = default;

    void Process(const NodePtr& root);

private:
    void visit(const NodePtr& node);
    void visitElementNode(const std::shared_ptr<ElementNode>& node);
};

} // namespace CHTL
