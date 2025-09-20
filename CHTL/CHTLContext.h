#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <map>
#include <string>
#include <memory>
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "NamespaceData.h"
#include "CHTLNode/AttributeNode.h" // For AttributeNode
#include <vector>

namespace CHTL {

class ElementNode; // Forward declaration

class CHTLContext {
public:
    CHTLContext();

    std::map<std::string, std::unique_ptr<BaseNode>> files;
    std::map<std::string, NamespaceData> namespaces;
    std::string current_namespace;

    void addTemplateDefinition(const TemplateDefinitionNode& def);
    const TemplateDefinitionNode* getTemplateDefinition(const std::string& name, const std::string& from_namespace = "");
    std::vector<AttributeNode> resolveStyles(ElementNode* element);
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
