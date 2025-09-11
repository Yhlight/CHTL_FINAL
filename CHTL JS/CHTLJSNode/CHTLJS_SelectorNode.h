#ifndef CHTLJS_SELECTOR_NODE_H
#define CHTLJS_SELECTOR_NODE_H

#include "CHTLJS_BaseNode.h"
#include <string>

// Represents an enhanced selector, e.g., {{.box}}
class CHTLJS_SelectorNode : public CHTLJS_BaseNode {
public:
    std::string selector;

    explicit CHTLJS_SelectorNode(const std::string& sel) : selector(sel) {}
};

#endif // CHTLJS_SELECTOR_NODE_H
