#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace CHTL {

// Forward-declare the class to break the include cycle
class TemplateDefinitionNode;

class Node {
public:
    virtual ~Node() = default;

    // For debugging and verification
    virtual std::string ToString() const = 0;
};

// Program is the root node of every AST our parser produces.
class Program : public Node {
public:
    std::string ToString() const override;

    std::vector<std::unique_ptr<Node>> statements;
    std::map<std::string, std::unique_ptr<TemplateDefinitionNode>> templateRegistry;
};

} // namespace CHTL
