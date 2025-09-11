#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include "../CHTLJSNode/CHTLJS_TextNode.h"
#include "../CHTLJSNode/CHTLJS_SelectorNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class CHTLJSGenerator {
public:
    std::string generate(const std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes, const std::string& parentSelector);

private:
    void visit(const CHTLJS_BaseNode* node, const std::string& parentSelector);
    void visitTextNode(const CHTLJS_TextNode* node);
    void visitSelectorNode(const CHTLJS_SelectorNode* node, const std::string& parentSelector);

    std::stringstream output;
};

#endif // CHTLJS_GENERATOR_H
