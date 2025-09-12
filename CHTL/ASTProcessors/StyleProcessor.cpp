#include "StyleProcessor.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../LocalStyleParser/LocalStyleParser.h"
#include "../CHTLContext.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <iostream>
#include <set>

void StyleProcessor::process(std::shared_ptr<BaseNode> root, CHTLContext& context) {
    if (root) {
        this->context = &context;
        root->accept(*this);
    }
}

// Recursive helper to expand templates and handle inheritance
void expandStyleTemplate(
    const std::string& templateName,
    CHTLContext& context,
    std::map<std::string, std::shared_ptr<ExprNode>>& targetMap,
    std::set<std::string>& visited)
{
    if (visited.count(templateName)) {
        std::cerr << "Error: Circular style template inheritance detected involving '" << templateName << "'." << std::endl;
        return;
    }
    if (!context.styleTemplates.count(templateName)) {
        std::cerr << "Error: Undefined style template '" << templateName << "'." << std::endl;
        return;
    }

    visited.insert(templateName);

    auto& templateNode = context.styleTemplates[templateName];

    // First, expand parent templates recursively
    for (const auto& baseTemplateName : templateNode->baseTemplates) {
        expandStyleTemplate(baseTemplateName, context, targetMap, visited);
    }

    // Then, merge this template's properties, overriding any from parents
    for (const auto& prop : templateNode->properties) {
        targetMap[prop.first] = prop.second;
    }

    visited.erase(templateName);
}


void StyleProcessor::visit(ElementNode& node) {
    currentElement = &node;

    LocalStyleParser styleParser;
    std::vector<std::shared_ptr<BaseNode>> remainingChildren;

    for (const auto& child : node.children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            ParsedStyleBlock parsedBlock = styleParser.parse(styleNode->rawContent);

            // 1. Expand and merge any used templates
            std::set<std::string> visitedTemplates;
            for (const auto& templateName : parsedBlock.templateUsages) {
                expandStyleTemplate(templateName, *context, node.unevaluatedStyles, visitedTemplates);
            }

            // 2. Merge the inline style trees, overriding template styles
            node.unevaluatedStyles.insert(parsedBlock.inlineStyleTrees.begin(), parsedBlock.inlineStyleTrees.end());

            // 3. Process global rules
            for (const auto& rule : parsedBlock.globalRules) {
                context->globalCssContent += rule.selector + " { " + rule.rawContent + " }\n";
                std::string selector = trim(rule.selector);
                if (selector.rfind('.', 0) == 0) {
                    std::string className = selector.substr(1);
                    if (node.attributes.count("class")) { node.attributes["class"] += " " + className; }
                    else { node.attributes["class"] = className; }
                } else if (selector.rfind('#', 0) == 0) {
                    node.attributes["id"] = selector.substr(1);
                }
            }
        } else {
            remainingChildren.push_back(child);
        }
    }

    node.children = remainingChildren;

    for (const auto& child : node.children) {
        child->accept(*this);
    }

    currentElement = nullptr;
}

void StyleProcessor::visit(TextNode& node) {}
void StyleProcessor::visit(CommentNode& node) {}
void StyleProcessor::visit(StyleNode& node) {}
void StyleProcessor::visit(StyleTemplateNode& node) {}
