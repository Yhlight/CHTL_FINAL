#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL {

class ScriptNode : public Node {
public:
    ScriptNode(std::string content) : content(std::move(content)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "ScriptNode( ... )";
    }

    NodePtr clone() const override {
        return std::make_shared<ScriptNode>(content);
    }

    std::string content;
};

} // namespace CHTL
