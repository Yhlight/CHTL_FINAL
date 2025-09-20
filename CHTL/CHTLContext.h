#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <map>
#include <string>
#include <memory>
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "NamespaceData.h"

namespace CHTL {

class CHTLContext {
public:
    CHTLContext();

    std::map<std::string, std::unique_ptr<BaseNode>> files;
    std::map<std::string, NamespaceData> namespaces;
    std::string current_namespace;

    void addTemplateDefinition(const TemplateDefinitionNode& def);
    const TemplateDefinitionNode* getTemplateDefinition(const std::string& name, const std::string& from_namespace = "");
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
