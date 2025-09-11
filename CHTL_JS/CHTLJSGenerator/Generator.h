#pragma once

#include "../CHTLJSNode/Node.h"
#include <string>
#include <sstream>

class CHTLJSGenerator : public CHTLJSVisitor {
public:
    std::string generate(const CHTLJSNodeList& ast);

    void visit(FileLoaderNode* node) override;
    void visit(ListenNode* node) override;
    void visit(AnimateNode* node) override;
    void visit(VirDeclNode* node) override;
    void visit(EnhancedSelectorNode* node) override;
    void visit(CHTLJSPropertyNode* node) override;
    void visit(MethodCallNode* node) override;

private:
    std::stringstream output;
};
