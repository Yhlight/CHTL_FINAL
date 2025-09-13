#include <iostream>

#include "ASTPrinter.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/ScriptBlockNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/PropertyReferenceNode.h"
#include "../CHTLNode/PropertyValue.h"
#include "../CHTLNode/AttributeNode.h"


namespace CHTL {

std::string ASTPrinter::print(const RootNode& root, std::shared_ptr<ParserContext> context) {
    out_.str(""); // Clear the stream

    out_ << "--- AST ---\n";
    printNode(&root, 0);

    out_ << "\n--- Templates ---\n";
    for (const auto& pair : context->element_templates_) {
        printElementTemplate(pair.second.get(), 0);
    }

    out_ << "\n--- Module Info ---\n";
    out_ << "Name: " << context->module_info_.name << "\n";
    out_ << "Version: " << context->module_info_.version << "\n";
    out_ << "Author: " << context->module_info_.author << "\n";

    out_ << "\n--- Exports ---\n";
    out_ << "Element Templates:\n";
    for (const auto& name : context->export_map_.element_templates) {
        out_ << "  - " << name << "\n";
    }
    out_ << "Style Templates:\n";
    for (const auto& name : context->export_map_.style_templates) {
        out_ << "  - " << name << "\n";
    }

    return out_.str();
}

void ASTPrinter::printIndentation(int indent) {
    for (int i = 0; i < indent; ++i) {
        out_ << "  ";
    }
}

void ASTPrinter::printNode(const Node* node, int indent) {
    if (!node) return;

    // Use dynamic_cast to determine the type and call the specific printer
    if (auto p = dynamic_cast<const RootNode*>(node)) {
        printIndentation(indent);
        out_ << "RootNode\n";
        for (const auto& child : p->children_) {
            printNode(child.get(), indent + 1);
        }
    } else if (auto p = dynamic_cast<const ElementNode*>(node)) {
        printElement(p, indent);
    } else if (auto p = dynamic_cast<const TextNode*>(node)) {
        printText(p, indent);
    } else if (auto p = dynamic_cast<const CommentNode*>(node)) {
        printComment(p, indent);
    } else if (auto p = dynamic_cast<const StyleBlockNode*>(node)) {
        printStyleBlock(p, indent);
    } else if (auto p = dynamic_cast<const CssRuleNode*>(node)) {
        printCssRule(p, indent);
    } else if (auto p = dynamic_cast<const OriginNode*>(node)) {
        printOrigin(p, indent);
    } else if (auto p = dynamic_cast<const ScriptBlockNode*>(node)) {
        printScriptBlock(p, indent);
    } else if (auto p = dynamic_cast<const ElementTemplateNode*>(node)) {
        printElementTemplate(p, indent);
    }
    // Add other node types here as needed
    else {
        printIndentation(indent);
        out_ << "UnknownNode\n";
    }
}

void ASTPrinter::printElement(const ElementNode* node, int indent) {
    printIndentation(indent);
    out_ << "ElementNode <" << node->tagName_ << ">\n";

    // Print attributes
    for (const auto& attr : node->attributes_) {
        printIndentation(indent + 1);
        out_ << "Attribute: " << attr->key_ << " = \"" << attr->value_ << "\"\n";
    }

    // Print children
    for (const auto& child : node->children_) {
        printNode(child.get(), indent + 1);
    }
}

void ASTPrinter::printText(const TextNode* node, int indent) {
    printIndentation(indent);
    // Escape newlines for cleaner printing
    std::string content = node->text_;
    size_t pos = 0;
    while ((pos = content.find('\n', pos)) != std::string::npos) {
        content.replace(pos, 1, "\\n");
        pos += 2;
    }
    out_ << "TextNode: \"" << content << "\"\n";
}

void ASTPrinter::printComment(const CommentNode* node, int indent) {
    printIndentation(indent);
    out_ << "CommentNode: \"" << node->comment_ << "\"\n";
}

void ASTPrinter::printStyleBlock(const StyleBlockNode* node, int indent) {
    printIndentation(indent);
    out_ << "StyleBlockNode\n";

    // Print inline properties
    for (const auto& prop : node->inline_properties_) {
        printIndentation(indent + 1);
        out_ << "Inline Prop: " << prop.first << " = ...\n"; // Simplified for now
    }

    // Print CSS rules
    for (const auto& rule : node->rules_) {
        printNode(rule.get(), indent + 1);
    }
}

void ASTPrinter::printCssRule(const CssRuleNode* node, int indent) {
    printIndentation(indent);
    out_ << "CssRuleNode: " << node->selector_ << "\n";
    for(const auto& prop : node->properties_) {
        printIndentation(indent + 1);
        // This part needs PropertyValue to be printable
        // For now, just print the key.
        out_ << "Property: " << prop.first << "\n";
    }
}

void ASTPrinter::printScriptBlock(const ScriptBlockNode* node, int indent) {
    printIndentation(indent);
    out_ << "ScriptBlockNode\n";
    printIndentation(indent + 1);
    out_ << "Content: \"" << node->content_ << "\"\n";
}

// Stubs for other node types - to be implemented
void ASTPrinter::printStyleTemplate(const StyleTemplateNode* node, int indent) {
    printIndentation(indent);
    out_ << "StyleTemplateNode (Unimplemented)\n";
}
void ASTPrinter::printElementTemplate(const ElementTemplateNode* node, int indent) {
    printIndentation(indent);
    out_ << "ElementTemplate: " << node->name_ << "\n";
    for (const auto& child : node->children_) {
        printNode(child.get(), indent + 1);
    }
}
void ASTPrinter::printVarTemplate(const VarTemplateNode* node, int indent) {
    printIndentation(indent);
    out_ << "VarTemplateNode (Unimplemented)\n";
}
void ASTPrinter::printOrigin(const OriginNode* node, int indent) {
    printIndentation(indent);
    out_ << "OriginNode type: " << node->type_ << "\n";
    printIndentation(indent + 1);
    out_ << "Content: \"" << node->content_ << "\"\n";
}
void ASTPrinter::printPropertyReference(const PropertyReferenceNode* node, int indent) {
    printIndentation(indent);
    out_ << "PropertyReferenceNode (Unimplemented)\n";
}


} // namespace CHTL
