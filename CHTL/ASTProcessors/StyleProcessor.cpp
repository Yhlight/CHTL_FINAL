#include "StyleProcessor.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../LocalStyleParser/LocalStyleParser.h"
#include "../CHTLContext.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <iostream>

void StyleProcessor::process(std::shared_ptr<BaseNode> root, CHTLContext& context) {
    if (root) {
        // We need to pass the context to the visit methods.
        // A simple way is to store it as a member before starting.
        this->context = &context;
        root->accept(*this);
    }
}

void StyleProcessor::visit(ElementNode& node) {
    currentElement = &node;

    LocalStyleParser styleParser;
    std::vector<std::shared_ptr<BaseNode>> remainingChildren;

    for (const auto& child : node.children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            // This is a style node, process it
            ParsedStyleBlock parsedBlock = styleParser.parse(styleNode->rawContent);

            // 1. Merge inline styles
            node.processedStyles.insert(parsedBlock.inlineStyles.begin(), parsedBlock.inlineStyles.end());

            // 2. Process global rules
            // First pass: Find a context selector (.class or #id) for the '&' operator
            std::string contextSelector;
            // Per spec, class is preferred over id for context
            for (const auto& rule : parsedBlock.globalRules) {
                std::string selector = trim(rule.selector);
                if (selector.rfind('.', 0) == 0) {
                    contextSelector = selector;
                    break; // Found class, stop looking
                }
            }
            if (contextSelector.empty()) { // No class found, look for an ID
                 for (const auto& rule : parsedBlock.globalRules) {
                    std::string selector = trim(rule.selector);
                    if (selector.rfind('#', 0) == 0) {
                        contextSelector = selector;
                        break; // Found ID, stop looking
                    }
                }
            }

            // Second pass: Process all rules
            for (const auto& rule : parsedBlock.globalRules) {
                std::string selector = trim(rule.selector);
                std::string finalSelector = selector;

                // 2a. Handle context deduction for '&'
                if (selector.rfind('&', 0) == 0) {
                    if (!contextSelector.empty()) {
                        finalSelector = contextSelector + selector.substr(1);
                    } else {
                        // Error/Warning: '&' used without a valid context selector
                        std::cerr << "Warning: '&' used without a class/id context on line (line info not available)." << std::endl;
                    }
                }

                // 2b. Add rule to global CSS content
                context->globalCssContent += finalSelector + " { " + rule.content + " }\n";

                // 2c. Automatically add class/id attributes (only for non-& selectors)
                if (selector.rfind('&', 0) != 0) {
                    if (selector.rfind('.', 0) == 0) { // Starts with .
                        std::string className = selector.substr(1);
                        if (node.attributes.count("class")) {
                            node.attributes["class"] += " " + className;
                        } else {
                            node.attributes["class"] = className;
                        }
                    } else if (selector.rfind('#', 0) == 0) { // Starts with #
                        std::string idName = selector.substr(1);
                        node.attributes["id"] = idName;
                    }
                }
            }
        } else {
            // This is not a style node, keep it
            remainingChildren.push_back(child);
        }
    }

    // Replace the old children list with the one that has StyleNodes removed
    node.children = remainingChildren;

    // Recursively visit the remaining children
    for (const auto& child : node.children) {
        child->accept(*this);
    }

    currentElement = nullptr;
}

void StyleProcessor::visit(TextNode& node) {
    // Do nothing
}

void StyleProcessor::visit(CommentNode& node) {
    // Do nothing
}

void StyleProcessor::visit(StyleNode& node) {
    // Should not be called directly.
}
