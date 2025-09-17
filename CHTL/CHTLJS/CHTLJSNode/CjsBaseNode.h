#pragma once

#include <memory>
#include <string>

// Forward declarations
class CjsSelectorNode;
class CjsPassthroughNode;

class CjsVisitor {
public:
    virtual ~CjsVisitor() = default;
    virtual void visit(CjsSelectorNode& node) = 0;
    virtual void visit(CjsPassthroughNode& node) = 0;
};

class CjsBaseNode {
public:
    virtual ~CjsBaseNode() = default;
    virtual void accept(CjsVisitor& visitor) = 0;

    std::string content;
};
