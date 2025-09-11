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
            for (const auto& rule : parsedBlock.globalRules) {
                // 2a. Add rule to global CSS content
                context->globalCssContent += rule.selector + " { " + rule.content + " }\n";

                // 2b. Automatically add class/id attributes
                std::string selector = trim(rule.selector);
                if (selector.rfind('.', 0) == 0) { // Starts with .
                    std::string className = selector.substr(1);
                    if (node.attributes.count("class")) {
                        node.attributes["class"] += " " + className;
                    } else {
                        node.attributes["class"] = className;
                    }
                } else if (selector.rfind('#', 0) == 0) { // Starts with #
                    std::string idName = selector.substr(1);
                    // Note: Overwrites if an ID already exists. This is reasonable.
                    node.attributes["id"] = idName;
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
