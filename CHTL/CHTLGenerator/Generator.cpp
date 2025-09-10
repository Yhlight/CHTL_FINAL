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

std::string Generator::generate(const BaseNode* root) {
    if (!root) {
        return "";
    }
    output.str(""); // Clear the stream for the main HTML body
    GeneratorContext context;

    visit(root, context);

    std::string finalHtml = output.str();
    std::string globalStyles = context.globalCss.str();

    if (!globalStyles.empty()) {
        std::string styleBlock = "<style>" + globalStyles + "</style>";
        // Insert style block before the closing </head> tag
        size_t headEndPos = finalHtml.find("</head>");
        if (headEndPos != std::string::npos) {
            finalHtml.insert(headEndPos, styleBlock);
        } else {
            // Fallback: just prepend to the document if no head tag is found
            finalHtml = styleBlock + finalHtml;
        }
    }

    return finalHtml;
}

void Generator::visit(const BaseNode* node, GeneratorContext& context) {
    if (const auto* elem = dynamic_cast<const ElementNode*>(node)) {
        visitElementNode(elem, context);
    } else if (const auto* text = dynamic_cast<const TextNode*>(node)) {
        visitTextNode(text, context);
    } else if (const auto* comment = dynamic_cast<const CommentNode*>(node)) {
        visitCommentNode(comment, context);
    }
    // StyleNode is handled within visitElementNode, so we don't visit it from here.
}

void Generator::visitElementNode(const ElementNode* node, GeneratorContext& context) {
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
    }

    // Process style blocks to generate inline styles
    std::stringstream inlineStyle;
    for (const auto& child : node->children) {
        if (const auto* style = dynamic_cast<const StyleNode*>(child.get())) {
            visitStyleNode(style, context, node); // This will populate global styles
            // Now, get inline styles from the same StyleNode
            for (const auto& styleChild : style->children) {
                if (const auto* decl = dynamic_cast<const DeclarationNode*>(styleChild.get())) {
                    inlineStyle << visitDeclarationNode(decl, context);
                }
            }
        }
    }

    std::string styleAttr = inlineStyle.str();
    if (!styleAttr.empty()) {
        output << " style=\"" << escapeHTML(styleAttr) << "\"";
    }

    output << ">";

    // Visit non-style children
    for (const auto& child : node->children) {
        if (!dynamic_cast<const StyleNode*>(child.get())) {
            visit(child.get(), context);
        }
    }

    output << "</" << node->tagName << ">";
}

void Generator::visitTextNode(const TextNode* node, GeneratorContext& context) {
    output << escapeHTML(node->content);
}

void Generator::visitCommentNode(const CommentNode* node, GeneratorContext& context) {
    output << "<!-- " << node->content << " -->";
}

void Generator::visitStyleNode(const StyleNode* node, GeneratorContext& context, const ElementNode* parent) {
    for (const auto& child : node->children) {
        if (const auto* rule = dynamic_cast<const RuleNode*>(child.get())) {
            visitRuleNode(rule, context, parent);
        }
        // Declarations are handled in visitElementNode for inline styles
    }
}

void Generator::visitRuleNode(const RuleNode* node, GeneratorContext& context, const ElementNode* parent) {
    // Format the selector
    std::string selector;
    for (const auto& token : node->selectorTokens) {
        if (token.type == TokenType::AMPERSAND) {
            // Handle '&' replacement with parent selector
            if (parent->attributes.count("class")) {
                std::string firstClass = parent->attributes.at("class");
                size_t spacePos = firstClass.find(" ");
                if (spacePos != std::string::npos) {
                    firstClass = firstClass.substr(0, spacePos);
                }
                selector += "." + firstClass;
            } else if (parent->attributes.count("id")) {
                selector += "#" + parent->attributes.at("id");
            }
            // else: '&' is ignored if no class/id context is found
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

std::string Generator::visitDeclarationNode(const DeclarationNode* node, GeneratorContext& context) {
    ExpressionParser parser(node->valueTokens);
    std::unique_ptr<ExprNode> exprTree = parser.parse();

    ExpressionEvaluator evaluator;
    std::string value = evaluator.evaluate(exprTree.get());

    return node->property + ": " + value + "; ";
}
