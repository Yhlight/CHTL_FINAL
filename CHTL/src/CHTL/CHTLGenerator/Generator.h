#pragma once

#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomUsageNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/OriginUsageNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <map>
#include <vector>

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
    void generateCustomUsage(const CustomUsageNode* node);
    void generateOrigin(const OriginNode* node);
    void generateOriginUsage(const OriginUsageNode* node);

    void expandStyleProperties(const BaseNode* container,
                               std::map<std::string, const StylePropertyNode*>& properties);

    // New helper to find the target for an insert rule
    std::vector<std::unique_ptr<BaseNode>>::iterator
        findInsertionPoint(const std::string& selector,
                           std::vector<std::unique_ptr<BaseNode>>& nodes);

    // Helper for advanced conditional expressions
    const ElementNode* findNodeBySelector(const BaseNode* searchRoot, const std::string& selector) const;


    std::string m_output;
    std::string m_global_css;
    const CHTLContext* m_context = nullptr;
    const DocumentNode* m_root_node = nullptr;
};

} // namespace CHTL
