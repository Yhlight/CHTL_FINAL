#ifndef CHTL_VAR_DECLARATION_NODE_H
#define CHTL_VAR_DECLARATION_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class VarDeclarationNode : public BaseNode {
public:
    std::string name;
    std::string value;
};

} // namespace CHTL

#endif // CHTL_VAR_DECLARATION_NODE_H
