#ifndef CHTL_JS_SCRIPT_LOADER_NODE_H
#define CHTL_JS_SCRIPT_LOADER_NODE_H

#include "CHTLJS/CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

class ScriptLoaderNode : public CHTLJSNode {
public:
    ScriptLoaderNode() : CHTLJSNode(CHTLJSNodeType::Scriptloader) {}

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<ScriptLoaderNode>();
        new_node->paths_ = this->paths_;
        return new_node;
    }

    std::vector<std::string> paths_;
};

} // namespace CHTLJS

#endif // CHTL_JS_SCRIPT_LOADER_NODE_H
