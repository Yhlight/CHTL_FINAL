#include "ChtlJsGenerator.h"
#include "../CHTLJSNode/CjsSelectorNode.h"
#include "../CHTLJSNode/CjsPassthroughNode.h"

std::string ChtlJsGenerator::generate(const std::vector<std::shared_ptr<CjsBaseNode>>& ast) {
    generatedCode.clear();
    for (const auto& node : ast) {
        node->accept(*this);
    }
    return generatedCode;
}

void ChtlJsGenerator::visit(CjsSelectorNode& node) {
    generatedCode += "document.querySelector('" + node.content + "')";
}

void ChtlJsGenerator::visit(CjsPassthroughNode& node) {
    generatedCode += node.content;
}
