#include "Generator.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigurationNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/UseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include "CHTLUtil/ASTCloner.h"
#include <vector>
#include <iostream>
#include <sstream>

class TemplateCollector : public Visitor {
public:
    TemplateCollector(
        std::map<std::string, Generator::StyleTemplateMap>& styleTemplates,
        std::map<std::string, Generator::ElementTemplateMap>& elementTemplates,
        std::map<std::string, Generator::VarTemplateMap>& varTemplates
    ) : styleTemplates(styleTemplates), elementTemplates(elementTemplates), varTemplates(varTemplates) {
        // Start in the global namespace
        namespace_stack.push_back("");
    }

    void collect(const NodeList& ast) {
        for (const auto& node : ast) {
            node->accept(*this);
        }
    }

    void visit(StyleTemplateDefinitionNode* node) override { styleTemplates[currentNamespace()][node->name] = node; }
    void visit(ElementTemplateDefinitionNode* node) override { elementTemplates[currentNamespace()][node->name] = node; }
    void visit(VarTemplateDefinitionNode* node) override { varTemplates[currentNamespace()][node->name] = node; }

    void visit(NamespaceNode* node) override {
        namespace_stack.push_back(node->name);
        for(const auto& child : node->children) child->accept(*this);
        namespace_stack.pop_back();
    }

    // Unchanged visit methods...
    void visit(ElementNode* n) override{ for(const auto& child : n->children) child->accept(*this); }
    void visit(TextNode* n) override{} void visit(CommentNode* n) override{} void visit(PropertyNode* n) override{} void visit(StyleNode* n) override{} void visit(ScriptNode* n) override{} void visit(StyleUsageNode* n) override{} void visit(ElementUsageNode* n) override{} void visit(ImportNode* n) override{} void visit(ConfigurationNode* n) override{} void visit(CustomStyleDefinitionNode* n) override{} void visit(CustomElementDefinitionNode* n) override{} void visit(CustomVarDefinitionNode* n) override{} void visit(DeleteNode* n) override{} void visit(InsertNode* n) override{} void visit(UseNode* n) override{} void visit(ConstraintNode* n) override{}

private:
    std::string currentNamespace() {
        // TODO: Handle nested namespaces (e.g., "space.room")
        return namespace_stack.back();
    }

    std::map<std::string, Generator::StyleTemplateMap>& styleTemplates;
    std::map<std::string, Generator::ElementTemplateMap>& elementTemplates;
    std::map<std::string, Generator::VarTemplateMap>& varTemplates;
    std::vector<std::string> namespace_stack;
};

std::string Generator::generate(NodeList& ast) {
    auto dummy_root = std::make_unique<ElementNode>("__DUMMY_ROOT__");
    for(auto& node : ast) {
        dummy_root->addChild(std::move(node));
    }
    this->ast_root = dummy_root.get();

    TemplateCollector collector(styleTemplates, elementTemplates, varTemplates);
    collector.collect(dummy_root->children);

    for (const auto& node : dummy_root->children) {
        if (dynamic_cast<TemplateDefinitionNode*>(node.get()) == nullptr && dynamic_cast<ConfigurationNode*>(node.get()) == nullptr) {
             if (auto useNode = dynamic_cast<UseNode*>(node.get())) {
                if (useNode->getTargetAsString() == "html5") {
                    output << "<!DOCTYPE html>\n";
                }
            }
            node->accept(*this);
        }
    }
    return output.str();
}

void Generator::visit(ElementNode* node) {
    if (node->tagName == "__DUMMY_ROOT__") {
        for(const auto& child : node->children) child->accept(*this);
        return;
    }
    printIndent();
    output << "<" << node->tagName;

    std::vector<PropertyNode*> properties;
    std::vector<StyleNode*> styleNodes;
    std::vector<ScriptNode*> scriptNodes;
    std::vector<ElementUsageNode*> elementUsages;
    std::vector<Node*> otherChildren;

    for (const auto& child : node->children) {
        if (auto* prop = dynamic_cast<PropertyNode*>(child.get())) properties.push_back(prop);
        else if (auto* style = dynamic_cast<StyleNode*>(child.get())) styleNodes.push_back(style);
        else if (auto* script = dynamic_cast<ScriptNode*>(child.get())) scriptNodes.push_back(script);
        else if (auto* usage = dynamic_cast<ElementUsageNode*>(child.get())) elementUsages.push_back(usage);
        else otherChildren.push_back(child.get());
    }

    ExprEvaluator evaluator(*ast_root, varTemplates);

    for (const auto* prop : properties) {
        std::string value = evaluator.evaluate(*prop->value).toString();
        output << " " << prop->name << "=\"" << value << "\"";
    }

    if (!styleNodes.empty()) {
        std::stringstream style_ss;
        for (const auto* styleNode : styleNodes) {
            for (const auto& styleChild : styleNode->children) {
                if (auto* styleProp = dynamic_cast<PropertyNode*>(styleChild.get())) {
                    std::string value = evaluator.evaluate(*styleProp->value).toString();
                    style_ss << styleProp->name << ":" << value << ";";
                } else if (auto* styleUsage = dynamic_cast<StyleUsageNode*>(styleChild.get())) {
                    std::string ns = styleUsage->namespace_name.empty() ? "" : styleUsage->namespace_name;
                    auto ns_it = styleTemplates.find(ns);
                    if (ns_it != styleTemplates.end()) {
                        auto it = ns_it->second.find(styleUsage->name);
                        if (it != ns_it->second.end()) {
                            for (const auto& prop : it->second->properties) {
                                std::string value = evaluator.evaluate(*prop->value).toString();
                                style_ss << prop->name << ":" << value << ";";
                            }
                        }
                    }
                }
            }
        }
        output << " style=\"" << style_ss.str() << "\"";
    }

    output << ">";

    bool hasContent = !otherChildren.empty() || !elementUsages.empty() || !scriptNodes.empty();
    if(hasContent) output << "\n";
    indent();
    for (auto* child : otherChildren) child->accept(*this);
    for (auto* usage : elementUsages) usage->accept(*this);
    for (auto* script : scriptNodes) script->accept(*this);
    dedent();
    if(hasContent) printIndent();

    output << "</" << node->tagName << ">\n";
}

void Generator::visit(TextNode* node) { printIndent(); output << node->text << "\n"; }
void Generator::visit(CommentNode* node) { if (node->isGeneratorComment) { printIndent(); output << "<!--" << node->text << "-->\n"; } }
void Generator::visit(PropertyNode* node) {}
void Generator::visit(StyleNode* node) {}
void Generator::visit(StyleTemplateDefinitionNode* node) {}
void Generator::visit(ElementTemplateDefinitionNode* node) {}
void Generator::visit(VarTemplateDefinitionNode* node) {}
void Generator::visit(StyleUsageNode* node) {}
void Generator::visit(ScriptNode* node) { printIndent(); output << "<script>" << node->placeholder << "</script>\n"; }

void Generator::visit(ElementUsageNode* node) {
    std::string ns = node->namespace_name.empty() ? "" : node->namespace_name;
    auto ns_it = elementTemplates.find(ns);
    if (ns_it != elementTemplates.end()) {
        auto it = ns_it->second.find(node->name);
        if (it != ns_it->second.end()) {
            ASTCloner cloner;
            NodeList clonedChildren = cloner.clone(it->second->children);
            for (const auto& spec : node->specializations) {
                if (auto* deleteNode = dynamic_cast<DeleteNode*>(spec.get())) {
                    int match_count = 0;
                    clonedChildren.erase(
                        std::remove_if(clonedChildren.begin(), clonedChildren.end(),
                            [&](const NodePtr& child) {
                                if (auto* elem = dynamic_cast<ElementNode*>(child.get())) {
                                    if (elem->tagName == deleteNode->target) {
                                        if (deleteNode->index == -1 || deleteNode->index == match_count) {
                                            match_count++;
                                            return true;
                                        }
                                        match_count++;
                                    }
                                }
                                return false;
                            }),
                        clonedChildren.end()
                    );
                } else if (auto* insertNode = dynamic_cast<InsertNode*>(spec.get())) {
                    if (insertNode->position == InsertNode::Position::AtBottom) {
                        for(auto& child : insertNode->children) {
                            clonedChildren.push_back(std::move(child));
                        }
                    } else if (insertNode->position == InsertNode::Position::AtTop) {
                        for(auto it_child = insertNode->children.rbegin(); it_child != insertNode->children.rend(); ++it_child) {
                            clonedChildren.insert(clonedChildren.begin(), std::move(*it_child));
                        }
                    }
                    // TODO: Implement other insert positions
                }
            }
            for (const auto& child : clonedChildren) child->accept(*this);
        }
    }
}

void Generator::visit(NamespaceNode* node) { for (const auto& child : node->children) child->accept(*this); }
void Generator::visit(UseNode* node) {}
void Generator::visit(ConstraintNode* node) {}
void Generator::visit(CustomStyleDefinitionNode* node) {}
void Generator::visit(CustomElementDefinitionNode* node) {}
void Generator::visit(CustomVarDefinitionNode* node) {}
void Generator::visit(DeleteNode* node) {}
void Generator::visit(InsertNode* node) {}
void Generator::visit(ImportNode* node) {}
void Generator::visit(ConfigurationNode* node) {}

void Generator::indent() { indent_level++; }
void Generator::dedent() { indent_level--; }
void Generator::printIndent() { for (int i = 0; i < indent_level; ++i) output << "  "; }
