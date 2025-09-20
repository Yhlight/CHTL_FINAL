#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>
#include <memory>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    std::string content;

    explicit ScriptNode(std::string content) : content(std::move(content)) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ScriptNode>(content);
    }
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H
