#include "CHTLGenerator.h"
#include <map>
#include <iostream>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<ParserContext> context)
    : context_(context) {}

// The main entry point for generation.
std::string CHTLGenerator::generate(const RootNode& root) {
    // Pass 1: Collect all static properties for referencing.
    for (const auto& child : root.children_) {
        collectProperties(child.get());
    }

    // Pass 2: Generate the HTML and CSS.
    for (const auto& child : root.children_) {
        visit(child.get());
    }

    std::string html = output_.str();
    std::string styles = global_styles_.str();

    if (!styles.empty()) {
        std::string style_tag = "\n    <style>\n" + styles + "    </style>\n";
        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            html.insert(head_pos, style_tag);
        } else {
            html = style_tag + html;
        }
    }

    return html;
}

// Pass 1: Recursively traverses the AST to find elements with IDs/classes
// and statically-defined properties, storing them in the registry.
void CHTLGenerator::collectProperties(const Node* node) {
    if (!node || node->getType() != NodeType::Element) {
        return;
    }

    const auto* elementNode = static_cast<const ElementNode*>(node);

    std::string id_selector;
    std::string class_selector;

    for (const auto& attr : elementNode->attributes_) {
        if (attr->key_ == "id") {
            id_selector = "#" + attr->value_;
        } else if (attr->key_ == "class") {
            // For simplicity, we'll only use the first class for the registry key.
            class_selector = "." + attr->value_.substr(0, attr->value_.find(" "));
        }
    }

    // Recurse for children first, so nested properties are collected.
    for (const auto& child : elementNode->children_) {
        collectProperties(child.get());
    }

    // Now process this node's style block.
    for (const auto& child : elementNode->children_) {
        if (child->getType() == NodeType::StyleBlock) {
            const auto* styleNode = static_cast<const StyleBlockNode*>(child.get());
            for (const auto& prop : styleNode->inline_properties_) {
                // A property is "static" if it has only one token of type Number.
                if (prop.second.size() == 1 && prop.second[0].type == TokenType::Number) {
                    try {
                        auto value_unit = CssValueEvaluator::parseValueUnit(prop.second[0].lexeme);
                        if (!id_selector.empty()) {
                            property_registry_[id_selector][prop.first] = value_unit;
                        }
                        if (!class_selector.empty()) {
                             property_registry_[class_selector][prop.first] = value_unit;
                        }
                    } catch (const std::exception& e) {
                        // Ignore properties that can't be parsed into a simple value.
                    }
                }
            }
        }
    }
}


// Pass 2: Main recursive visitor for generating the output.
void CHTLGenerator::visit(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element:
            visitElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            visitText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            visitComment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::Origin:
            visitOrigin(static_cast<const OriginNode*>(node));
            break;
        case NodeType::StyleBlock:
            // Style blocks are handled by their parent element, so we do nothing here.
            break;
        default:
            break;
    }
}

void CHTLGenerator::visitElement(const ElementNode* node) {
    std::map<std::string, std::string> attributes;
    for (const auto& attr : node->attributes_) {
        attributes[attr->key_] = attr->value_;
    }

    std::stringstream inline_style_ss;
    for (const auto& child : node->children_) {
        if (child->getType() == NodeType::StyleBlock) {
            const auto* styleNode = static_cast<const StyleBlockNode*>(child.get());

            // Build a local context for this element's style block
            CssValueEvaluator::LocalContext local_context;
            for (const auto& prop : styleNode->inline_properties_) {
                if (prop.second.size() == 1 && prop.second[0].type == TokenType::Number) {
                    try {
                        local_context[prop.first] = CssValueEvaluator::parseValueUnit(prop.second[0].lexeme);
                    } catch (...) { /* ignore */ }
                }
            }

            // Render inline styles using the local context
            for (const auto& prop : styleNode->inline_properties_) {
                const auto& tokens = prop.second;
                if (tokens.empty()) continue;

                bool has_expression = false;
                for (size_t i = 0; i < tokens.size(); ++i) {
                    if ((tokens[i].type >= TokenType::Plus && tokens[i].type <= TokenType::DoubleAsterisk) || tokens[i].type == TokenType::QuestionMark) {
                        has_expression = true;
                        break;
                    }
                    if (tokens[i].type == TokenType::Identifier && (i + 1) < tokens.size() &&
                        tokens[i+1].type == TokenType::Identifier && !tokens[i+1].lexeme.empty() &&
                        tokens[i+1].lexeme[0] == '.') {
                        has_expression = true;
                        break;
                    }
                }

                inline_style_ss << prop.first << ": ";
                if (has_expression) {
                    CssValueEvaluator evaluator;
                    inline_style_ss << evaluator.evaluate(tokens, property_registry_, local_context);
                } else {
                    for (size_t i = 0; i < tokens.size(); ++i) {
                        inline_style_ss << tokens[i].lexeme << (i < tokens.size() - 1 ? " " : "");
                    }
                }
                inline_style_ss << "; ";
            }

            // Render rules (they don't have a local context)
            CssValueEvaluator::LocalContext empty_local_context;
            for (const auto& rule : styleNode->rules_) {
                std::string final_selector = rule->selector_;
                if (rule->selector_[0] == '&') {
                    std::string context_selector;
                    if (attributes.count("id")) {
                        context_selector = "#" + attributes["id"];
                    } else if (attributes.count("class")) {
                        context_selector = "." + attributes["class"].substr(0, attributes["class"].find(" "));
                    }
                    if (!context_selector.empty()) {
                        final_selector.replace(0, 1, context_selector);
                    }
                }

                global_styles_ << "      " << final_selector << " {\n";
                for (const auto& prop : rule->properties_) {
                     const auto& tokens = prop.second;
                    if (tokens.empty()) continue;

                    bool has_expression = false;
                    for (size_t i = 0; i < tokens.size(); ++i) {
                        if ((tokens[i].type >= TokenType::Plus && tokens[i].type <= TokenType::DoubleAsterisk) || tokens[i].type == TokenType::QuestionMark) {
                            has_expression = true;
                            break;
                        }
                        if (tokens[i].type == TokenType::Identifier && (i + 1) < tokens.size() &&
                            tokens[i+1].type == TokenType::Identifier && !tokens[i+1].lexeme.empty() &&
                            tokens[i+1].lexeme[0] == '.') {
                            has_expression = true;
                            break;
                        }
                    }

                    global_styles_ << "        " << prop.first << ": ";
                    if (has_expression) {
                        CssValueEvaluator evaluator;
                        global_styles_ << evaluator.evaluate(tokens, property_registry_, empty_local_context);
                    } else {
                        for (size_t i = 0; i < tokens.size(); ++i) {
                            global_styles_ << tokens[i].lexeme << (i < tokens.size() - 1 ? " " : "");
                        }
                    }
                    global_styles_ << ";\n";
                }
                global_styles_ << "      }\n";

                if (rule->selector_[0] == '.' && !context_->config_.DISABLE_STYLE_AUTO_ADD_CLASS) {
                    std::string className = rule->selector_.substr(1);
                    if (attributes.count("class")) {
                        if ((" " + attributes["class"] + " ").find(" " + className + " ") == std::string::npos) {
                            attributes["class"] += " " + className;
                        }
                    } else {
                        attributes["class"] = className;
                    }
                } else if (rule->selector_[0] == '#' && !context_->config_.DISABLE_STYLE_AUTO_ADD_ID) {
                    attributes["id"] = rule->selector_.substr(1);
                }
            }
        }
    }

    std::string inline_style_attr = inline_style_ss.str();
    if (!inline_style_attr.empty()) {
        attributes["style"] = inline_style_attr;
    }

    indent();
    output_ << "<" << node->tagName_;
    for(const auto& attr_pair : attributes) {
        output_ << " " << attr_pair.first << "=\"" << attr_pair.second << "\"";
    }

    bool hasNonStyleChildren = false;
    for (const auto& child : node->children_) {
        if (child->getType() != NodeType::StyleBlock) {
            hasNonStyleChildren = true;
            break;
        }
    }

    if (!hasNonStyleChildren) {
        output_ << " />\n";
    } else {
        output_ << ">\n";
        indentLevel_++;
        for (const auto& child : node->children_) {
            if (child->getType() != NodeType::StyleBlock) {
                 visit(child.get());
            }
        }
        indentLevel_--;
        indent();
        output_ << "</" << node->tagName_ << ">\n";
    }
}

void CHTLGenerator::visitText(const TextNode* node) {
    indent();
    output_ << node->text_ << "\n";
}

void CHTLGenerator::visitComment(const CommentNode* node) {
    indent();
    output_ << "<!-- " << node->comment_ << " -->\n";
}

void CHTLGenerator::visitOrigin(const OriginNode* node) {
    output_ << node->content_;
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << "  ";
    }
}

} // namespace CHTL
