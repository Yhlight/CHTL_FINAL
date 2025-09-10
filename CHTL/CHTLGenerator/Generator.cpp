#include "Generator.h"
#include "ExprGenerator.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateNode.h"
#include <vector>
#include <iostream>
#include <sstream>

// --- Pre-pass Visitor to collect template definitions ---
class TemplateCollector : public Visitor {
public:
    TemplateCollector(
        std::map<std::string, StyleTemplateDefinitionNode*>& styleTemplates,
        std::map<std::string, ElementTemplateDefinitionNode*>& elementTemplates,
        std::map<std::string, VarTemplateDefinitionNode*>& varTemplates
    ) : styleTemplates(styleTemplates), elementTemplates(elementTemplates), varTemplates(varTemplates) {}

    void collect(const NodeList& ast) {
        for (const auto& node : ast) {
            node->accept(*this);
        }
    }

    void visit(StyleTemplateDefinitionNode* node) override { styleTemplates[node->name] = node; }
    void visit(ElementTemplateDefinitionNode* node) override { elementTemplates[node->name] = node; }
    void visit(VarTemplateDefinitionNode* node) override { varTemplates[node->name] = node; }

    // Ignore other nodes in this pass
    void visit(ElementNode* node) override {}
    void visit(TextNode* node) override {}
    void visit(CommentNode* node) override {}
    void visit(PropertyNode* node) override {}
    void visit(StyleNode* node) override {}
    void visit(StyleUsageNode* node) override {}
    void visit(ElementUsageNode* node) override {}

private:
    std::map<std::string, StyleTemplateDefinitionNode*>& styleTemplates;
    std::map<std::string, ElementTemplateDefinitionNode*>& elementTemplates;
    std::map<std::string, VarTemplateDefinitionNode*>& varTemplates;
};


// --- Main Generator ---

std::string Generator::generate(const NodeList& ast) {
    // Pass 1: Collect templates
    TemplateCollector collector(styleTemplates, elementTemplates, varTemplates);
    collector.collect(ast);

    // Pass 2: Generate output
    for (const auto& node : ast) {
        // Don't generate output for template definitions themselves
        if (dynamic_cast<TemplateDefinitionNode*>(node.get()) == nullptr) {
            node->accept(*this);
        }
    }
    return output.str();
}

void Generator::visit(ElementNode* node) {
    printIndent();
    output << "<" << node->tagName;

    std::vector<PropertyNode*> properties;
    std::vector<StyleNode*> styleNodes;
    std::vector<ElementUsageNode*> elementUsages;
    std::vector<Node*> otherChildren;

    for (const auto& child : node->children) {
        if (auto* prop = dynamic_cast<PropertyNode*>(child.get())) {
            properties.push_back(prop);
        } else if (auto* style = dynamic_cast<StyleNode*>(child.get())) {
            styleNodes.push_back(style);
        } else if (auto* usage = dynamic_cast<ElementUsageNode*>(child.get())) {
            elementUsages.push_back(usage);
        }
        else {
            otherChildren.push_back(child.get());
        }
    }

    // Add normal attributes
    for (const auto* prop : properties) {
        ExprGenerator exprGen;
        std::string value = exprGen.generate(prop->value.get());
        output << " " << prop->name << "=\"" << value << "\"";
    }

    // Collect and add inline styles
    if (!styleNodes.empty()) {
        std::stringstream style_ss;
        for (const auto* styleNode : styleNodes) {
            for (const auto& styleChild : styleNode->children) {
                if (auto* styleProp = dynamic_cast<PropertyNode*>(styleChild.get())) {
                    ExprGenerator exprGen;
                    std::string value = exprGen.generate(styleProp->value.get());
                    style_ss << styleProp->name << ":" << value << ";";
                } else if (auto* styleUsage = dynamic_cast<StyleUsageNode*>(styleChild.get())) {
                    // Handle @Style usage inside a style block
                    auto it = styleTemplates.find(styleUsage->name);
                    if (it != styleTemplates.end()) {
                        for (const auto& prop : it->second->properties) {
                            ExprGenerator exprGen;
                            std::string value = exprGen.generate(prop->value.get());
                            style_ss << prop->name << ":" << value << ";";
                        }
                    }
                }
            }
        }
        output << " style=\"" << style_ss.str() << "\"";
    }

    output << ">";

    if (!otherChildren.empty() || !elementUsages.empty()) {
        output << "\n";
        indent();
        // Visit other children
        for (auto* child : otherChildren) {
            child->accept(*this);
        }
        // Visit element usages
        for (auto* usage : elementUsages) {
            usage->accept(*this);
        }
        dedent();
        printIndent();
    }

    output << "</" << node->tagName << ">\n";
}

void Generator::visit(TextNode* node) {
    printIndent();
    output << node->text << "\n";
}

void Generator::visit(CommentNode* node) {
    if (node->isGeneratorComment) {
        printIndent();
        output << "<!--" << node->text << "-->\n";
    }
}

void Generator::visit(PropertyNode* node) {}
void Generator::visit(StyleNode* node) {}
void Generator::visit(StyleTemplateDefinitionNode* node) {}
void Generator::visit(ElementTemplateDefinitionNode* node) {}
void Generator::visit(VarTemplateDefinitionNode* node) {}

void Generator::visit(StyleUsageNode* node) {
    // This is handled inside visit(ElementNode*)
}

void Generator::visit(ElementUsageNode* node) {
    auto it = elementTemplates.find(node->name);
    if (it != elementTemplates.end()) {
        // Render the children of the found template
        for (const auto& child : it->second->children) {
            child->accept(*this);
        }
    }
}


void Generator::indent() {
    indent_level++;
}

void Generator::dedent() {
    indent_level--;
}

void Generator::printIndent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "  ";
    }
}
