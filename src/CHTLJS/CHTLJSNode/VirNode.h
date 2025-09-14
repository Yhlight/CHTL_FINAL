#ifndef CHTL_JS_VIR_NODE_H
#define CHTL_JS_VIR_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <memory>

namespace CHTLJS {

class VirNode : public CHTLJSNode {
public:
    VirNode(std::string name, std::unique_ptr<CHTLJSNode> value)
        : name_(std::move(name)), value_(std::move(value)) {}

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Vir; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        return std::make_unique<VirNode>(name_, value_->clone());
    }

    const std::string& getName() const { return name_; }
    const CHTLJSNode* getValue() const { return value_.get(); }

private:
    std::string name_;
    std::unique_ptr<CHTLJSNode> value_; // The CHTL JS function node (e.g., ListenNode)
};

} // namespace CHTLJS

#endif // CHTL_JS_VIR_NODE_H
