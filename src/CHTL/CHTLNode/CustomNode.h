#ifndef CHTL_CUSTOMNODE_H
#define CHTL_CUSTOMNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CustomNode : public BaseNode {
public:
    // Basic structure, will be expanded later
    explicit CustomNode(std::string name) : name(std::move(name)) {}

    void accept(NodeVisitor& visitor) override;

private:
    std::string name;
};

} // namespace CHTL

#endif // CHTL_CUSTOMNODE_H
