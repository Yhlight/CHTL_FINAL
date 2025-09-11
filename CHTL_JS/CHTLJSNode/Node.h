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
class CHTLJSPropertyNode;
class MethodCallNode;
class CHTLJSVisitor;

// Base class for all CHTL JS AST nodes
class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
    virtual void accept(CHTLJSVisitor& visitor) = 0;
};

using CHTLJSNodePtr = std::unique_ptr<CHTLJSNode>;
using CHTLJSNodeList = std::vector<CHTLJSNodePtr>;

// Visitor interface for the CHTL JS AST
class CHTLJSVisitor {
public:
    virtual ~CHTLJSVisitor() = default;
    virtual void visit(FileLoaderNode* node) = 0;
    virtual void visit(ListenNode* node) = 0;
    virtual void visit(AnimateNode* node) = 0;
    virtual void visit(VirDeclNode* node) = 0;
    virtual void visit(EnhancedSelectorNode* node) = 0;
    virtual void visit(CHTLJSPropertyNode* node) = 0;
    virtual void visit(MethodCallNode* node) = 0;
};

// Represents a method call like `{{box}}->listen { ... }`
class MethodCallNode : public CHTLJSNode {
public:
    MethodCallNode(CHTLJSNodePtr callee, const std::string& methodName, CHTLJSNodePtr arguments)
        : callee(std::move(callee)), methodName(methodName), arguments(std::move(arguments)) {}

    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }

    CHTLJSNodePtr callee;
    std::string methodName;
    CHTLJSNodePtr arguments; // For now, a single argument which is the block
};


// Represents a `vir` variable declaration
class VirDeclNode : public CHTLJSNode {
public:
    VirDeclNode(const std::string& name, CHTLJSNodePtr value)
        : name(name), value(std::move(value)) {}

    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }

    std::string name;
    CHTLJSNodePtr value;
};

// Represents an enhanced selector like {{box}}
class EnhancedSelectorNode : public CHTLJSNode {
public:
    EnhancedSelectorNode(const std::string& selector) : selector(selector) {}
    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }
    std::string selector;
};

// Represents a key-value pair inside a CHTL JS block
class CHTLJSPropertyNode : public CHTLJSNode {
public:
    CHTLJSPropertyNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}
    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }

    std::string key;
    std::string value;
};

// Represents a `listen { ... }` block
class ListenNode : public CHTLJSNode {
public:
    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }
    std::vector<std::unique_ptr<CHTLJSPropertyNode>> eventHandlers;
};

// Represents an `animate { ... }` block
class AnimateNode : public CHTLJSNode {
public:
    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }
    std::vector<std::unique_ptr<CHTLJSPropertyNode>> properties;
};

// Represents a `fileloader { ... }` block
class FileLoaderNode : public CHTLJSNode {
public:
    void accept(CHTLJSVisitor& visitor) override { visitor.visit(this); }
    std::vector<std::string> files;
};
