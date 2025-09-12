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
            // Parse the style block into expression trees and raw global rules
            ParsedStyleBlock parsedBlock = styleParser.parse(styleNode->rawContent);

            // 1. Move the unevaluated inline style trees to the element node
            node.unevaluatedStyles.insert(parsedBlock.inlineStyleTrees.begin(), parsedBlock.inlineStyleTrees.end());

            // 2. Process global rules
            for (const auto& rule : parsedBlock.globalRules) {
                // For now, just hoist the raw content. Evaluation of values inside
                // global rules will be handled by the StyleEvaluator pass as well.
                context->globalCssContent += rule.selector + " { " + rule.rawContent + " }\n";

                // Automatically add class/id attributes
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
                    node.attributes["id"] = idName;
                }
            }
        } else {
            remainingChildren.push_back(child);
        }
    }

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
