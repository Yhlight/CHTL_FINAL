#ifndef CHTL_JS_SEQUENCE_NODE_H
#define CHTL_JS_SEQUENCE_NODE_H

#include "CHTLJSNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

class SequenceNode : public CHTLJSNode {
public:
    SequenceNode() : CHTLJSNode(CHTLJSNodeType::Sequence) {}

    std::vector<std::unique_ptr<CHTLJSNode>> statements_;
};

} // namespace CHTLJS

#endif // CHTL_JS_SEQUENCE_NODE_H
