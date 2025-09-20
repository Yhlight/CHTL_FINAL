#ifndef CHTLJS_SELECTOR_NODE_H
#define CHTLJS_SELECTOR_NODE_H

#include "CHTLJS_BaseNode.h"
#include <string>
#include <memory>

namespace CHTLJS {

class CHTLJS_SelectorNode : public CHTLJS_BaseNode {
public:
    std::string selector;

    explicit CHTLJS_SelectorNode(std::string sel) : selector(std::move(sel)) {}

    void accept(CHTLJS_Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<CHTLJS_BaseNode> clone() const override {
        return std::make_unique<CHTLJS_SelectorNode>(selector);
    }
};

} // namespace CHTLJS

#endif // CHTLJS_SELECTOR_NODE_H
