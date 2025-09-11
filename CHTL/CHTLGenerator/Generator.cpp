#include "Generator.h"
#include "../CHTLExpressionParser/ExpressionParser.h"
#include "../CHTLExpressionEvaluator/ExpressionEvaluator.h"
#include <stdexcept>
#include <algorithm>

// Helper function for HTML escaping
std::string escapeHTML(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

std::string Generator::generate(const BaseNode* root, CHTLContext& context) {
    if (!root) return "";
    output.str("");
    visit(root, context);
    std::string finalHtml = output.str();
    std::string globalStyles = context.globalCss.str();
    if (!globalStyles.empty()) {
        std::string styleBlock = "<style>" + globalStyles + "</style>";
        size_t headEndPos = finalHtml.find("</head>");
        if (headEndPos != std::string::npos) {
            finalHtml.insert(headEndPos, styleBlock);
        } else {
            finalHtml = styleBlock + finalHtml;
        }
    }
    return finalHtml;
}

void Generator::visit(const BaseNode* node, CHTLContext& context) {
    if (const auto* elem = dynamic_cast<const ElementNode*>(node)) visitElementNode(elem, context);
    else if (const auto* text = dynamic_cast<const TextNode*>(node)) visitTextNode(text, context);
    else if (const auto* comment = dynamic_cast<const CommentNode*>(node)) visitCommentNode(comment, context);
    else if (const auto* usage = dynamic_cast<const TemplateUsageNode*>(node)) visitTemplateUsageNode(usage, context, nullptr);
}

void Generator::visitElementNode(const ElementNode* node, CHTLContext& context) {
    output << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
    }

    std::stringstream inlineStyle;
    // Two-pass evaluation for self-referencing properties
    for (const auto& child : node->children) {
        if (const auto* style = dynamic_cast<const StyleNode*>(child.get())) {
            PropertyMap localProps;
            ExpressionEvaluator evaluator; // Create one evaluator for the whole block
            // First pass: evaluate simple declarations to populate context
            for (const auto& styleChild : style->children) {
                if (const auto* decl = dynamic_cast<const DeclarationNode*>(styleChild.get())) {
                    // A "simple" declaration is a single number or keyword, optionally with a unit.
                    if (decl->valueTokens.size() == 1 || (decl->valueTokens.size() == 2 && decl->valueTokens[0].type == TokenType::NUMBER && decl->valueTokens[1].type == TokenType::IDENTIFIER)) {
                        const auto& firstToken = decl->valueTokens[0];
                        if (firstToken.type == TokenType::NUMBER) {
                             if (decl->valueTokens.size() > 1) {
                                localProps[decl->property] = CssValue{ std::stod(firstToken.lexeme), decl->valueTokens[1].lexeme };
                             } else {
                                localProps[decl->property] = CssValue{ std::stod(firstToken.lexeme), "" };
                             }
                        } else {
                            localProps[decl->property] = CssValue{ firstToken.lexeme, "" };
                        }
                    }
                }
            }
            // Second pass: evaluate all declarations
            for (const auto& styleChild : style->children) {
                if (const auto* decl = dynamic_cast<const DeclarationNode*>(styleChild.get())) {
                    ExpressionParser parser(decl->valueTokens);
                    auto expr = parser.parse();
                    inlineStyle << decl->property << ": " << evaluator.evaluate(expr.get(), localProps) << "; ";
                } else if (const auto* rule = dynamic_cast<const RuleNode*>(styleChild.get())) {
                    visitRuleNode(rule, context, node);
                } else if (const auto* usage = dynamic_cast<const TemplateUsageNode*>(styleChild.get())) {
                    inlineStyle << visitTemplateUsageNode(usage, context, node);
                }
            }
        }
    }

    std::string styleAttr = inlineStyle.str();
    if (!styleAttr.empty()) {
        output << " style=\"" << escapeHTML(styleAttr) << "\"";
    }

    output << ">";

    for (const auto& child : node->children) {
        if (!dynamic_cast<const StyleNode*>(child.get())) {
            visit(child.get(), context);
        }
    }
    output << "</" << node->tagName << ">";
}

void Generator::visitTextNode(const TextNode* node, CHTLContext& context) { output << escapeHTML(node->content); }
void Generator::visitCommentNode(const CommentNode* node, CHTLContext& context) { output << "<!-- " << node->content << " -->"; }
void Generator::visitStyleNode(const StyleNode* node, CHTLContext& context, const ElementNode* parent) {}

void Generator::visitRuleNode(const RuleNode* node, CHTLContext& context, const ElementNode* parent) {
    std::string selector;
    for (const auto& token : node->selectorTokens) {
        if (token.type == TokenType::AMPERSAND) {
            if (parent && parent->attributes.count("class")) {
                std::string firstClass = parent->attributes.at("class");
                size_t spacePos = firstClass.find(" ");
                if (spacePos != std::string::npos) firstClass = firstClass.substr(0, spacePos);
                selector += "." + firstClass;
            } else if (parent && parent->attributes.count("id")) {
                selector += "#" + parent->attributes.at("id");
            }
        } else {
            selector += token.lexeme;
        }
    }

    context.globalCss << selector << " { ";
    for (const auto& decl : node->declarations) {
        context.globalCss << visitDeclarationNode(decl.get(), context);
    }
    context.globalCss << "} ";
}

std::string Generator::visitDeclarationNode(const DeclarationNode* node, CHTLContext& context) {
    ExpressionParser parser(node->valueTokens);
    auto expr = parser.parse();
    ExpressionEvaluator evaluator;
    PropertyMap emptyContext;
    std::string value = evaluator.evaluate(expr.get(), emptyContext);
    return node->property + ": " + value + "; ";
}

std::string Generator::visitTemplateUsageNode(const TemplateUsageNode* node, CHTLContext& context, const ElementNode* parent) {
    TemplateNode* templateDef = nullptr;
    if (node->type == TemplateType::STYLE) {
        if (context.styleTemplates.count(node->name)) templateDef = context.styleTemplates.at(node->name);
    } else if (node->type == TemplateType::ELEMENT) {
        if (context.elementTemplates.count(node->name)) templateDef = context.elementTemplates.at(node->name);
    }

    if (templateDef) {
        std::stringstream templateOutput;
        for (const auto& child : templateDef->body) {
            if (node->type == TemplateType::STYLE) {
                if (const auto* decl = dynamic_cast<const DeclarationNode*>(child.get())) {
                    templateOutput << visitDeclarationNode(decl, context);
                } else if (const auto* rule = dynamic_cast<const RuleNode*>(child.get())) {
                    visitRuleNode(rule, context, parent);
                }
            } else {
                visit(child.get(), context);
            }
        }
        return templateOutput.str();
    }
    return "";
}
