#include "BaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
#include <sstream>

namespace CHTL
{
    // Helper to create indentation
    std::string indent_str(int level) {
        return std::string(level * 2, ' ');
    }

    // ProgramNode implementation
    std::string ProgramNode::toString(int indent) const {
        std::stringstream ss;
        ss << indent_str(indent) << "Program\n";
        for (const auto& child : children) {
            ss << child->toString(indent + 1);
        }
        return ss.str();
    }

    // ElementNode implementation
    void ElementNode::addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void ElementNode::addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    std::string ElementNode::toString(int indent) const {
        std::stringstream ss;
        ss << indent_str(indent) << "Element: <" << tagName << ">\n";
        if (!attributes.empty()) {
            ss << indent_str(indent + 1) << "Attributes:\n";
            for (const auto& attr : attributes) {
                ss << indent_str(indent + 2) << attr.first << " = \"" << attr.second << "\"\n";
            }
        }
        if (!children.empty()) {
            ss << indent_str(indent + 1) << "Children:\n";
            for (const auto& child : children) {
                ss << child->toString(indent + 2);
            }
        }
        return ss.str();
    }

    // TextNode implementation
    std::string TextNode::toString(int indent) const {
        std::stringstream ss;
        ss << indent_str(indent) << "Text: \"" << content << "\"\n";
        return ss.str();
    }

    // CommentNode implementation
    std::string CommentNode::toString(int indent) const {
        std::stringstream ss;
        ss << indent_str(indent) << "Comment: \"" << content << "\"\n";
        return ss.str();
    }

    // StyleNode implementation
    void StyleNode::addInlineProperty(const std::string& key, std::unique_ptr<ExprNode> value) {
        inline_properties[key] = std::move(value);
    }

    void StyleNode::addRule(CssRule&& rule) {
        rules.push_back(std::move(rule));
    }

    std::string StyleNode::toString(int indent) const {
        std::stringstream ss;
        ss << indent_str(indent) << "Style\n";
        if (!inline_properties.empty()) {
            ss << indent_str(indent + 1) << "Inline Properties:\n";
            for (const auto& prop : inline_properties) {
                ss << indent_str(indent + 2) << prop.first << ": " << (prop.second ? prop.second->toString() : "nullptr") << "\n";
            }
        }
        if (!rules.empty()) {
            ss << indent_str(indent + 1) << "Rules:\n";
            for (const auto& rule : rules) {
                ss << indent_str(indent + 2) << "Selector: " << rule.selector << "\n";
                for (const auto& prop : rule.properties) {
                    ss << indent_str(indent + 3) << prop.first << ": " << (prop.second ? prop.second->toString() : "nullptr") << "\n";
                }
            }
        }
        return ss.str();
    }
}
