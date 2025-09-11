#include "StyleProcessor.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../LocalStyleParser/LocalStyleParser.h"
#include <algorithm>

void StyleProcessor::process(std::shared_ptr<BaseNode> root) {
    if (root) {
        root->accept(*this);
    }
}

void StyleProcessor::visit(ElementNode& node) {
    // Set the current element context for child nodes
    currentElement = &node;

    // First, process any direct StyleNode children
    LocalStyleParser styleParser;
    for (const auto& child : node.children) {
        // We need to check the type before visiting.
        // A dynamic_cast is a safe way to do this.
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            auto parsedStyles = styleParser.parse(styleNode->rawContent);
            // Merge the parsed styles into the element's style map
            node.processedStyles.insert(parsedStyles.begin(), parsedStyles.end());
        }
    }

    // Remove all StyleNode children after processing them
    node.children.erase(
        std::remove_if(node.children.begin(), node.children.end(),
            [](const std::shared_ptr<BaseNode>& child) {
                return std::dynamic_pointer_cast<StyleNode>(child) != nullptr;
            }),
        node.children.end()
    );

    // Now, recursively visit the remaining children
    for (const auto& child : node.children) {
        child->accept(*this);
    }

    // Reset the context when we leave the element
    currentElement = nullptr;
}

void StyleProcessor::visit(TextNode& node) {
    // Do nothing
}

void StyleProcessor::visit(CommentNode& node) {
    // Do nothing
}

void StyleProcessor::visit(StyleNode& node) {
    // This method should ideally not be called directly if the logic
    // in visit(ElementNode&) is correct, as StyleNodes are processed
    // and removed there. We include it to satisfy the visitor interface.
}
