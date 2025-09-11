#ifndef CHTL_TEMPLATESTORE_H
#define CHTL_TEMPLATESTORE_H

#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <memory>
#include <map>

class TemplateStore {
public:
    // Adds a template definition to the store under a given namespace.
    // The key will be stored as "namespace.name".
    bool add(const std::string& ns, std::shared_ptr<TemplateDefinitionNode> templateNode);

    // Retrieves a template definition by name, searching in the given namespace and the global scope.
    std::shared_ptr<TemplateDefinitionNode> get(const std::string& ns, const std::string& name) const;

private:
    std::string getQualifiedName(const std::string& ns, const std::string& name) const;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_store;
};

#endif //CHTL_TEMPLATESTORE_H
