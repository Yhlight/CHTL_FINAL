#ifndef IMPORT_NODE_H
#define IMPORT_NODE_H

#include <string>
#include <vector>

namespace CHTL {

// Represents an [Import] statement.
// This node is transient and used by the parser to kick off a sub-parse.
// It doesn't become a permanent part of the final document AST.
struct ImportNode {
    std::string path;
    // ... other fields for specific vs. wildcard imports can be added later
};

} // namespace CHTL

#endif // IMPORT_NODE_H
