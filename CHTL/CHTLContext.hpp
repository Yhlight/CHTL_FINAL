#pragma once

#include "CHTLNode/BaseNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TemplateDefinitionNode.hpp"
#include <map>
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLContext {
public:
    void collect(const std::vector<NodePtr>& nodes);

    // Storage for templates, now public
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> elementTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> styleTemplates;

private:
    void collectNode(const NodePtr& node);
};

} // namespace CHTL
