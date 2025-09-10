#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/CSSRuleNode.h"
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <sstream>
#include "../CHTLStore/TemplateStore.h"
#include "../CHTLNode/TemplateUsageNode.h"

class Generator {
public:
    // Constructor takes the template store
    explicit Generator(const TemplateStore& templateStore);

    // Main entry point. Takes the AST and returns the generated HTML.
    std::string generate(const std::vector<std::unique_ptr<BaseNode>>& program);

private:
    const TemplateStore& m_templateStore;
    // Buffer for collecting CSS rules for the global stylesheet
    std::stringstream m_global_styles;

    // Main dispatch function
    std::string generateNode(const BaseNode* node);

    // Specific node generation functions
    std::string generateElement(const ElementNode* node);
    std::string generateText(const TextNode* node);
    std::string generateComment(const CommentNode* node);
    std::string generateTemplateUsage(const TemplateUsageNode* node);
    void processStyleBody(const std::vector<std::unique_ptr<BaseNode>>& body, ElementNode* parent, std::stringstream& inline_style_stream);

    // A set of HTML tags that are self-closing (e.g., <img>, <br>)
    static const std::set<std::string> s_selfClosingTags;
};

#endif //CHTL_GENERATOR_H
