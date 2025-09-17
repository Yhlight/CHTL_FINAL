#include "Generator.h"
#include <stdexcept>

namespace CHTL {

Generator::Generator(std::shared_ptr<BaseNode> root) : m_root(std::move(root)) {}

void Generator::generate() {
    visit(m_root);
}

std::string Generator::getHtml() const {
    return m_html_stream.str();
}

std::string Generator::getCss() const {
    return m_css_stream.str();
}

std::string Generator::generateValue(const std::shared_ptr<BaseNode>& valueNode) {
    if (auto literal = std::dynamic_pointer_cast<LiteralNode>(valueNode)) {
        return literal->value;
    }
    if (auto varUsage = std::dynamic_pointer_cast<VarUsageNode>(valueNode)) {
        auto& registry = TemplateRegistry::getInstance();
        auto varGroup = registry.lookupTemplate(varUsage->groupName);
        if (varGroup && varGroup->templateType == TemplateType::VAR) {
            for (const auto& var_decl_base : varGroup->body) {
                if (auto var_decl = std::dynamic_pointer_cast<VarDeclarationNode>(var_decl_base)) {
                    if (var_decl->name == varUsage->varName) {
                        return var_decl->value;
                    }
                }
            }
        }
        return "VAR_NOT_FOUND";
    }
    return "UNKNOWN_VALUE_TYPE";
}

void Generator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    if (auto rootNode = std::dynamic_pointer_cast<RootNode>(node)) {
        visitRootNode(rootNode);
    } else if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(elementNode);
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(textNode);
    } else if (std::dynamic_pointer_cast<StyleNode>(node)) {
        // Handled by parent
    } else if (std::dynamic_pointer_cast<TemplateNode>(node)) {
        // Definitions produce no output
    } else if (auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
        if (usageNode->templateType == TemplateType::ELEMENT) {
            auto& registry = TemplateRegistry::getInstance();
            auto templateNode = registry.lookupTemplate(usageNode->name);
            if (templateNode) {
                for (const auto& bodyNode : templateNode->body) {
                    visit(bodyNode);
                }
            }
        }
    } else {
        throw std::runtime_error("Unknown node type in generator.");
    }
}

void Generator::visitRootNode(const std::shared_ptr<RootNode>& node) {
    for (const auto& child : node->children) {
        visit(child);
    }
}

void Generator::expandStyleNode(std::stringstream& stream, const std::shared_ptr<StyleNode>& styleNode) {
    for (const auto& style_child : styleNode->children) {
        if (auto propNode = std::dynamic_pointer_cast<CssPropertyNode>(style_child)) {
            stream << propNode->key << ": " << generateValue(propNode->value) << ";";
        } else if (auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(style_child)) {
            if (usageNode->templateType == TemplateType::STYLE) {
                auto& registry = TemplateRegistry::getInstance();
                auto templateNode = registry.lookupTemplate(usageNode->name);
                if (templateNode && templateNode->templateType == usageNode->templateType) {
                    for (const auto& template_body_node : templateNode->body) {
                        if (auto templateStyleNode = std::dynamic_pointer_cast<StyleNode>(template_body_node)) {
                            // Recursive call to handle nested inheritance
                            expandStyleNode(stream, templateStyleNode);
                        }
                    }
                }
            }
        }
    }
}

void Generator::visitElementNode(const std::shared_ptr<ElementNode>& node) {
    std::string indent(m_indent_level * 2, ' ');
    m_html_stream << indent << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        m_html_stream << " " << attr->key << "=\"" << generateValue(attr->value) << "\"";
    }

    std::stringstream inline_style_stream;
    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            expandStyleNode(inline_style_stream, styleNode);
        }
    }
    if (inline_style_stream.str().length() > 0) {
        m_html_stream << " style=\"" << inline_style_stream.str() << "\"";
    }

    m_html_stream << ">\n";

    m_indent_level++;
    for (const auto& child : node->children) {
        if (!std::dynamic_pointer_cast<StyleNode>(child)) {
            visit(child);
        }
    }
    m_indent_level--;

    m_html_stream << indent << "</" << node->tagName << ">\n";
}

void Generator::visitTextNode(const std::shared_ptr<TextNode>& node) {
    std::string indent(m_indent_level * 2, ' ');
    m_html_stream << indent << node->content << "\n";
}

} // namespace CHTL
