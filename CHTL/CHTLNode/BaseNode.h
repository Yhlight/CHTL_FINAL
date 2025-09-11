#pragma once

#include <memory>
#include <vector>

namespace CHTL {

// Forward declaration
class CHTLGenerator;
class BaseNode;

// Use smart pointers for memory management of AST nodes
using NodePtr = std::shared_ptr<BaseNode>;
using NodeList = std::vector<NodePtr>;

enum class NodeType {
    Element,
    Text,
    Comment,
    Style,
    TemplateDefinition,
    ElementTemplateUsage,
    ElementModification,
    ElementDeletion,
    ElementInsertion,
    Origin,
    Import,
    // ... other node types to be added later
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType GetType() const = 0;
    virtual NodePtr Clone() const = 0;
    // We can add a virtual `Accept(Visitor* visitor)` method here later
    // for the generator (Visitor design pattern).
};

} // namespace CHTL
