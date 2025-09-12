#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL {

class DeletePropertyNode : public Node {
public:
    DeletePropertyNode(std::string name) : property_name(std::move(name)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "DeleteProperty( " + property_name + " )";
    }

    NodePtr clone() const override {
        return std::make_shared<DeletePropertyNode>(property_name);
    }

    std::string property_name;
};

} // namespace CHTL
