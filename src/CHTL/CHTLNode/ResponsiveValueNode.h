#ifndef CHTL_RESPONSIVE_VALUE_NODE_H
#define CHTL_RESPONSIVE_VALUE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ResponsiveValueNode : public Node {
public:
    explicit ResponsiveValueNode(std::string variable_name) : variable_name_(std::move(variable_name)) {}

    NodeType getType() const override { return NodeType::ResponsiveValue; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<ResponsiveValueNode>(variable_name_);
    }

    const std::string& getVariableName() const { return variable_name_; }

private:
    std::string variable_name_;
};

} // namespace CHTL

#endif // CHTL_RESPONSIVE_VALUE_NODE_H
