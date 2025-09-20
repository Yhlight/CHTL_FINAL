#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <map>
#include <string>
#include <memory>
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"

namespace CHTL {

class CHTLContext {
public:
    CHTLContext();

    std::map<std::string, std::unique_ptr<BaseNode>> files;
    std::map<std::string, TemplateDefinitionNode> template_definitions;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
