#ifndef CHTL_IMPORTNODE_H
#define CHTL_IMPORTNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents an [Import] statement
class ImportNode : public BaseNode {
public:
    explicit ImportNode(std::string path) : path(std::move(path)) {}

    void accept(NodeVisitor& visitor) override;

private:
    std::string path;
    // Other fields like 'as', specific items to import, etc., will be added later.
};

} // namespace CHTL

#endif // CHTL_IMPORTNODE_H
