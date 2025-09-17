#include "Generator.h"
#include "../CHTLNode/VarSubstitutionNode.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

// Forward declaration for recursive call
void generateStyleString(std::shared_ptr<Node> node, CHTLContext& context, std::stringstream& ss);


Generator::Generator(CHTLContext& context) : m_context(context) {}

std::string Generator::generate(std::shared_ptr<Node> node) {
    m_output.str("");
    m_output.clear();
    visit(node);
    return m_output.str();
}

void Generator::visit(std::shared_ptr<Node> node) {
    if (!node) return;

    if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(elementNode);
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(textNode.get());
    } else if (auto instantiationNode = std::dynamic_pointer_cast<TemplateInstantiationNode>(node)) {
        visitTemplateInstantiationNode(instantiationNode.get());
    }
}

void Generator::visitTemplateInstantiationNode(TemplateInstantiationNode* node) {
    auto templateDef = m_context.findTemplate(node->m_name.literal);
    if (templateDef) {
        if (node->m_templateType.type == TokenType::AT_ELEMENT) {
            for (const auto& child : templateDef->children) {
                visit(child);
            }
        }
    }
}


void Generator::visitElementNode(std::shared_ptr<ElementNode> node) {
    if (node->m_tagName == "root") {
        for (const auto& child : node->children) {
            visit(child);
        }
        return;
    }

    m_output << "<" << node->m_tagName;

    for (const auto& attr : node->attributes) {
        m_output << " " << attr->m_key << "=\"" << attr->m_value->toString(0) << "\"";
    }

    if (node->styleBlock && !node->styleBlock->children.empty()) {
        m_output << " style=\"";
        for (const auto& child : node->styleBlock->children) {
            generateStyleString(child, m_context, m_output);
        }
        m_output << "\"";
    }

    m_output << ">";

    for (const auto& child : node->children) {
        visit(child);
    }

    m_output << "</" << node->m_tagName << ">";
}

void Generator::visitTextNode(TextNode* node) {
    m_output << node->m_value;
}


// Helper function to recursively generate style strings
void generateStyleString(std::shared_ptr<Node> node, CHTLContext& context, std::stringstream& ss) {
    if (auto attr = std::dynamic_pointer_cast<AttributeNode>(node)) {
        ss << attr->m_key << ":";
        if (auto varSub = std::dynamic_pointer_cast<VarSubstitutionNode>(attr->m_value)) {
            auto varGroup = context.findTemplate(varSub->m_groupName.literal);
            if (varGroup) {
                for (const auto& varAttr : varGroup->children) {
                    if (auto concreteVar = std::dynamic_pointer_cast<AttributeNode>(varAttr)) {
                        if (concreteVar->m_key == varSub->m_variableName.literal) {
                            ss << concreteVar->m_value->toString(0);
                            break;
                        }
                    }
                }
            }
        } else if (attr->m_value) { // Placeholder attributes have no value
            ss << attr->m_value->toString(0);
        }
        ss << ";";
    } else if (auto inst = std::dynamic_pointer_cast<TemplateInstantiationNode>(node)) {
        if (inst->m_templateType.type == TokenType::AT_STYLE) {
            auto templateDef = context.findTemplate(inst->m_name.literal);
            if (templateDef) {
                for (const auto& child : templateDef->children) {
                    generateStyleString(child, context, ss);
                }
            }
        }
    } else if (auto customInst = std::dynamic_pointer_cast<CustomInstantiationNode>(node)) {
         if (customInst->m_templateType.type == TokenType::AT_STYLE) {
            auto templateDef = context.findTemplate(customInst->m_name.literal);
            if (templateDef) {
                for (const auto& basePropStmt : templateDef->children) {
                    if (auto baseProp = std::dynamic_pointer_cast<AttributeNode>(basePropStmt)) {
                        bool overridden = false;
                        for (const auto& overridePropStmt : customInst->children) {
                            if (auto overrideProp = std::dynamic_pointer_cast<AttributeNode>(overridePropStmt)) {
                                if (baseProp->m_key == overrideProp->m_key) {
                                    generateStyleString(overrideProp, context, ss);
                                    overridden = true;
                                    break;
                                }
                            }
                        }
                        if (!overridden && baseProp->m_value) {
                            generateStyleString(baseProp, context, ss);
                        }
                    }
                }
            }
        }
    }
}

} // namespace CHTL
