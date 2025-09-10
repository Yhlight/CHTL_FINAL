#pragma once

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <vector>
#include <sstream>

class CHTLJSGenerator {
public:
    CHTLJSGenerator(const std::vector<CHTLJSNodePtr>& ast);
    std::string generate();

private:
    const std::vector<CHTLJSNodePtr>& ast;
    std::stringstream output;

    void generateNode(const CHTLJSNodePtr& node);
    void visitListenNode(ListenNode* node);
    void visitSelectorNode(SelectorNode* node); // Might not be called directly
};
