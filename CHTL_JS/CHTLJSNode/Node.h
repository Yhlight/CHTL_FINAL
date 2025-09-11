#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLJSLexer/Token.h"

// Forward Declarations
class CHTLJSNode;
class FileLoaderNode;
class ListenNode;
class AnimateNode;
class VirDeclNode;
class EnhancedSelectorNode;

// Base class for all CHTL JS AST nodes
class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
};

using CHTLJSNodePtr = std::unique_ptr<CHTLJSNode>;
using CHTLJSNodeList = std::vector<CHTLJSNodePtr>;


// Represents a `vir` variable declaration
class VirDeclNode : public CHTLJSNode {
public:
    VirDeclNode(const std::string& name, CHTLJSNodePtr value)
        : name(name), value(std::move(value)) {}

    std::string name;
    CHTLJSNodePtr value;
};

// Represents an enhanced selector like {{box}}
class EnhancedSelectorNode : public CHTLJSNode {
public:
    EnhancedSelectorNode(const std::string& selector) : selector(selector) {}
    std::string selector;
};

// Represents a key-value pair inside a CHTL JS block
class CHTLJSPropertyNode : public CHTLJSNode {
public:
    // The value can be complex (e.g., a JS function body), so we store it as a string for now.
    // The Unified Scanner will eventually separate this out.
    CHTLJSPropertyNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    std::string key;
    std::string value;
};

// Represents a `listen { ... }` block
class ListenNode : public CHTLJSNode {
public:
    std::vector<std::unique_ptr<CHTLJSPropertyNode>> eventHandlers;
};

// Represents an `animate { ... }` block
class AnimateNode : public CHTLJSNode {
public:
    std::vector<std::unique_ptr<CHTLJSPropertyNode>> properties;
};

// Represents a `fileloader { ... }` block
class FileLoaderNode : public CHTLJSNode {
public:
    std::vector<std::string> files;
};
