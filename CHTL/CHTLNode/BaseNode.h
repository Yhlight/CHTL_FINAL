#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

// Base class for all Abstract Syntax Tree nodes.
// It's an abstract class, ensuring that only concrete node types can be instantiated.
class BaseNode {
public:
    // Virtual destructor is essential for a polymorphic base class.
    // It ensures that the derived class's destructor is called when deleting a base class pointer.
    virtual ~BaseNode() = default;
};

#endif // CHTL_BASE_NODE_H
