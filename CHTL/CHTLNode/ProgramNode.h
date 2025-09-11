#pragma once

#include "Node.h"
#include <vector>
#include <sstream>

namespace CHTL {

class ProgramNode : public Node {
public:
    ProgramNode() = default;

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "Program [\n";
        for (const auto& node : statements) {
            ss << node->ToString(indent + 2) << "\n";
        }
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    NodeList statements;
};

} // namespace CHTL
