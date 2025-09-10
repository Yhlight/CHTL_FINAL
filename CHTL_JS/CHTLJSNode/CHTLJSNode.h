#pragma once

#include <string>
#include <vector>
#include <memory>

class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
};

using CHTLJSNodePtr = std::unique_ptr<CHTLJSNode>;

// Represents an enhanced selector like {{.my-button}}
class SelectorNode : public CHTLJSNode {
public:
    std::string selector_text;
    SelectorNode(const std::string& text) : selector_text(text) {}
};

// Represents a listen { ... } block
class ListenNode : public CHTLJSNode {
public:
    CHTLJSNodePtr target; // The selector node
    // For now, we'll store the body as a raw string.
    // A full implementation would parse the key-value pairs of events and handlers.
    std::string body;
};
