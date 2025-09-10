#ifndef CHTL_TEMPLATESTORE_H
#define CHTL_TEMPLATESTORE_H

#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <memory>
#include <map>

class TemplateStore {
public:
    // Adds a template definition to the store.
    // Returns false if a template with the same name already exists.
    bool add(std::shared_ptr<TemplateDefinitionNode> templateNode);

    // Retrieves a template definition by name.
    // Returns nullptr if the template is not found.
    std::shared_ptr<TemplateDefinitionNode> get(const std::string& name) const;

private:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_store;
};

#endif //CHTL_TEMPLATESTORE_H
