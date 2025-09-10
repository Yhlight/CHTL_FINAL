#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include "../CHTLLexer/Token.h"
#include <string>
#include <memory>
#include <vector>

// Forward declarations for node types to resolve circular dependencies
class ElementNode;
class TextNode;
class AttributeNode;
class CommentNode;
// Add other node types here as they are created

class BaseNode {
public:
    // The token that this node was created from. Useful for error reporting.
    Token token;

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~BaseNode() = default;

    // A pure virtual function for debugging, forcing derived classes to implement it.
    // The 'override' keyword in derived classes will ensure it's implemented.
    virtual std::string debugString(int indent = 0) const = 0;

protected:
    // Helper for indentation in debug strings
    std::string indentString(int indent) const {
        return std::string(indent * 2, ' '); // 2 spaces per indent level
    }
};

#endif //CHTL_BASENODE_H
