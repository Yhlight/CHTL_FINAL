#pragma once

#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomUsageNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <map>

namespace CHTL {

class Generator {
public:
    Generator() = default;

    std::string generate(const DocumentNode* document, const CHTLContext& context);

private:
    void generateNode(const BaseNode* node);
    void generateElement(const ElementNode* element);
    void generateText(const TextNode* text);
    void generateTemplateUsage(const TemplateUsageNode* node, const ElementNode* parentElement);
    void generateCustomUsage(const CustomUsageNode* node); // Moved to be a member function

    void expandStyleProperties(const BaseNode* container,
                               std::map<std::string, const StylePropertyNode*>& properties);

    std::string m_output;
    std::string m_global_css;
    const CHTLContext* m_context = nullptr;
};

} // namespace CHTL
