#ifndef CHTL_JS_VIR_NODE_H
#define CHTL_JS_VIR_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <memory>

namespace CHTLJS {

class VirNode : public CHTLJSNode {
public:
    VirNode(const std::string& name, std::unique_ptr<CHTLJSNode> value)
        : CHTLJSNode(CHTLJSNodeType::Vir), name_(name), value_(std::move(value)) {}

    const std::string& getName() const { return name_; }
    const CHTLJSNode* getValue() const { return value_.get(); }

private:
    std::string name_;
    std::unique_ptr<CHTLJSNode> value_;
};

} // namespace CHTLJS

#endif // CHTL_JS_VIR_NODE_H
