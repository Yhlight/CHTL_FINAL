#include "Generator.h"
#include <stdexcept>
#include <algorithm>

namespace CHTL {

// --- New Selector Engine ---
struct Selector {
    std::string tagName;
    int index = 0;

    Selector(const std::string& s) {
        size_t bracket_pos = s.find('[');
        if (bracket_pos != std::string::npos) {
            tagName = s.substr(0, bracket_pos);
            size_t end_bracket_pos = s.find(']');
            if (end_bracket_pos != std::string::npos && end_bracket_pos > bracket_pos) {
                std::string index_str = s.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
                try {
                    index = std::stoi(index_str);
                } catch (...) {
                    index = 0;
                }
            }
        } else {
            tagName = s;
        }
    }
};

std::vector<std::shared_ptr<BaseNode>>::iterator findTarget(
    std::vector<std::shared_ptr<BaseNode>>& nodes,
    const Selector& selector)
{
    int count = 0;
    return std::find_if(nodes.begin(), nodes.end(), [&](const std::shared_ptr<BaseNode>& n) {
        if (auto en = std::dynamic_pointer_cast<ElementNode>(n)) {
            if (en->tagName == selector.tagName) {
                if (count == selector.index) {
                    return true;
                }
                count++;
            }
        }
        return false;
    });
}
// --- End Selector Engine ---


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
        auto varGroupDef = registry.lookupDefinition(varUsage->groupName);
        if (auto varGroup = std::dynamic_pointer_cast<TemplateNode>(varGroupDef)) {
            if (varGroup->templateType == TemplateType::VAR) {
                for (const auto& var_decl_base : varGroup->body) {
                    if (auto var_decl = std::dynamic_pointer_cast<VarDeclarationNode>(var_decl_base)) {
                        if (var_decl->name == varUsage->varName) {
                            return var_decl->value;
                        }
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
    } else if (std::dynamic_pointer_cast<TemplateNode>(node) || std::dynamic_pointer_cast<CustomNode>(node)) {
    } else if (auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
        if (usageNode->templateType == TemplateType::ELEMENT) {
            auto& registry = TemplateRegistry::getInstance();
            auto templateDef = registry.lookupDefinition(usageNode->name);
            if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(templateDef)) {
                for (const auto& bodyNode : templateNode->body) {
                    visit(bodyNode);
                }
            }
        }
    } else if (auto customUsage = std::dynamic_pointer_cast<CustomUsageNode>(node)) {
        if (customUsage->usageType == TemplateType::ELEMENT) {
            auto& registry = TemplateRegistry::getInstance();
            auto baseDef = registry.lookupDefinition(customUsage->name);
            if (baseDef) {
                std::vector<std::shared_ptr<BaseNode>> baseBody;
                if (auto tn = std::dynamic_pointer_cast<TemplateNode>(baseDef)) baseBody = tn->body;
                else if (auto cn = std::dynamic_pointer_cast<CustomNode>(baseDef)) baseBody = cn->body;

                std::vector<std::shared_ptr<BaseNode>> clonedBody;
                for(const auto& n : baseBody) {
                    clonedBody.push_back(n->clone());
                }

                for (const auto& spec_node : customUsage->specializationBody) {
                    if (auto insertNode = std::dynamic_pointer_cast<InsertNode>(spec_node)) {
                        Selector selector(insertNode->selector);
                        auto targetIt = findTarget(clonedBody, selector);

                        if (targetIt != clonedBody.end()) {
                            if (insertNode->position == InsertPosition::AFTER) {
                                clonedBody.insert(targetIt + 1, insertNode->body.begin(), insertNode->body.end());
                            } else if (insertNode->position == InsertPosition::BEFORE) {
                                clonedBody.insert(targetIt, insertNode->body.begin(), insertNode->body.end());
                            } else if (insertNode->position == InsertPosition::REPLACE) {
                                targetIt = clonedBody.erase(targetIt);
                                clonedBody.insert(targetIt, insertNode->body.begin(), insertNode->body.end());
                            } else if (insertNode->position == InsertPosition::AT_TOP) {
                                auto targetElement = std::dynamic_pointer_cast<ElementNode>(*targetIt);
                                if(targetElement) targetElement->children.insert(targetElement->children.begin(), insertNode->body.begin(), insertNode->body.end());
                            } else if (insertNode->position == InsertPosition::AT_BOTTOM) {
                                auto targetElement = std::dynamic_pointer_cast<ElementNode>(*targetIt);
                                if(targetElement) targetElement->children.insert(targetElement->children.end(), insertNode->body.begin(), insertNode->body.end());
                            }
                        }
                    }
                }
                for (const auto& bodyNode : clonedBody) {
                    visit(bodyNode);
                }
            }
        }
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
                auto templateDef = registry.lookupDefinition(usageNode->name);
                if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(templateDef)) {
                    if (templateNode->templateType == usageNode->templateType) {
                        for (const auto& template_body_node : templateNode->body) {
                            if (auto templateStyleNode = std::dynamic_pointer_cast<StyleNode>(template_body_node)) {
                                expandStyleNode(stream, templateStyleNode);
                            }
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
            for (const auto& style_child : styleNode->children) {
                if (auto customUsage = std::dynamic_pointer_cast<CustomUsageNode>(style_child)) {
                    auto& registry = TemplateRegistry::getInstance();
                    auto baseDef = registry.lookupDefinition(customUsage->name);
                    if (baseDef) {
                        std::shared_ptr<StyleNode> styleBodyToProcess;
                         if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(baseDef)) {
                            if (!templateNode->body.empty()) styleBodyToProcess = std::dynamic_pointer_cast<StyleNode>(templateNode->body[0]);
                        } else if (auto customNode = std::dynamic_pointer_cast<CustomNode>(baseDef)) {
                            if (!customNode->body.empty()) styleBodyToProcess = std::dynamic_pointer_cast<StyleNode>(customNode->body[0]);
                        }

                        if (styleBodyToProcess) {
                            auto clonedStyleNode = std::dynamic_pointer_cast<StyleNode>(styleBodyToProcess->clone());
                            if (clonedStyleNode) {
                                for (const auto& spec_node : customUsage->specializationBody) {
                                    if (auto deleteNode = std::dynamic_pointer_cast<DeleteNode>(spec_node)) {
                                        for (const auto& target : deleteNode->targets) {
                                            clonedStyleNode->children.erase(
                                                std::remove_if(clonedStyleNode->children.begin(), clonedStyleNode->children.end(),
                                                    [&](const std::shared_ptr<BaseNode>& n) {
                                                        if (auto p = std::dynamic_pointer_cast<CssPropertyNode>(n)) return p->key == target;
                                                        return false;
                                                    }),
                                                clonedStyleNode->children.end());
                                        }
                                    }
                                }
                                expandStyleNode(inline_style_stream, clonedStyleNode);
                            }
                        }
                    }
                } else {
                    auto tempStyleNode = std::make_shared<StyleNode>();
                    tempStyleNode->children.push_back(style_child);
                    expandStyleNode(inline_style_stream, tempStyleNode);
                }
            }
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
