#ifndef CHTL_TEMPLATENODE_H
#define CHTL_TEMPLATENODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TemplateNode : public BaseNode {
public:
    // Basic structure, will be expanded later
    explicit TemplateNode(std::string name) : name(std::move(name)) {}

    void accept(NodeVisitor& visitor) override;

private:
    std::string name;
};

} // namespace CHTL

#endif // CHTL_TEMPLATENODE_H
