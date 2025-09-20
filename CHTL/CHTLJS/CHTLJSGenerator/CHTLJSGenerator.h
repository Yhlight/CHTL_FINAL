#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../CHTLJSNode/CHTLJS_Visitor.h"
#include "../CHTLJSNode/CHTLJS_SelectorNode.h"
#include "../CHTLJSNode/CHTLJS_ListenNode.h"
#include "../CHTLJSNode/CHTLJS_RawJSNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

class CHTLJSGenerator : public CHTLJS_Visitor {
public:
    std::string generate(const std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes);

    void visit(CHTLJS_SelectorNode& node) override;
    void visit(CHTLJS_ListenNode& node) override;
    void visit(CHTLJS_RawJSNode& node) override;

private:
    std::string output;
};

} // namespace CHTLJS

#endif // CHTLJS_GENERATOR_H
