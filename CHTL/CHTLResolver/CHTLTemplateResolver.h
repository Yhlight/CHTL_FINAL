#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <map>

namespace CHTL {

class CHTLTemplateResolver {
public:
    CHTLTemplateResolver() = default;

    void Process(const std::shared_ptr<ProgramNode>& program);

private:
    void collectTemplates(const NodePtr& node);
    NodePtr expandTemplates(const NodePtr& node);

    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_template_registry;
};

} // namespace CHTL
