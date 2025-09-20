#ifndef NAMESPACE_DATA_H
#define NAMESPACE_DATA_H

#include "CHTLNode/TemplateDefinitionNode.h"
#include <map>
#include <string>

namespace CHTL {

struct NamespaceData {
    std::map<std::string, TemplateDefinitionNode> template_definitions;
    // ... other symbol maps will go here in the future (e.g., for custom elements, variables)
};

} // namespace CHTL

#endif // NAMESPACE_DATA_H
