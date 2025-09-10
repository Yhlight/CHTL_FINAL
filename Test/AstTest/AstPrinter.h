#ifndef CHTL_AST_PRINTER_H
#define CHTL_AST_PRINTER_H

#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include <iostream>
#include <string>

// Helper class to print the AST for debugging purposes.
class AstPrinter {
public:
    void print(const BaseNode* node) {
        if (!node) {
            std::cout << "AST is null." << std::endl;
            return;
        }
        printNode(node, "", true);
    }

private:
    void printNode(const BaseNode* node, const std::string& prefix, bool isLast) {
        if (!node) return;

        std::cout << prefix;
        std::cout << (isLast ? "└───" : "├───");

        // Dynamic cast to find out the concrete type of the node.
        if (const auto* elem = dynamic_cast<const ElementNode*>(node)) {
            std::cout << "Element: <" << elem->tagName << ">" << std::endl;
            // Print attributes
            for (const auto& attr : elem->attributes) {
                std::cout << prefix << (isLast ? "    " : "│   ") << "    ├───" << "Attr: " << attr.first << " = \"" << attr.second << "\"" << std::endl;
            }
            // Print children
            for (size_t i = 0; i < elem->children.size(); ++i) {
                printNode(elem->children[i].get(), prefix + (isLast ? "    " : "│   "), i == elem->children.size() - 1);
            }
        } else if (const auto* text = dynamic_cast<const TextNode*>(node)) {
            std::cout << "Text: \"" << text->content << "\"" << std::endl;
        } else if (const auto* comment = dynamic_cast<const CommentNode*>(node)) {
            std::cout << "Comment: \"" << comment->content << "\"" << std::endl;
        } else {
            std::cout << "Unknown Node" << std::endl;
        }
    }
};

#endif // CHTL_AST_PRINTER_H
