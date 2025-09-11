#include "Generator.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLExpressionParser/ExpressionParser.h"
#include "../CHTLExpressionEvaluator/ExpressionEvaluator.h"
#include "/app/CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "/app/CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <stdexcept>
#include <algorithm>
#include <atomic>

// Helper for generating unique IDs
std::atomic<int> id_counter(0);
std::string generateUniqueId() {
    return "chtl-id-" + std::to_string(++id_counter);
}

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
        if (headEndPos != std::string::npos) finalHtml.insert(headEndPos, styleBlock);
        else finalHtml = styleBlock + finalHtml;
    }

    std::string globalJs = context.globalJs.str();
    if (!globalJs.empty()) {
        std::string scriptBlock = "<script>" + globalJs + "</script>";
        size_t bodyEndPos = finalHtml.find("</body>");
        if (bodyEndPos != std::string::npos) finalHtml.insert(bodyEndPos, scriptBlock);
        else finalHtml += scriptBlock;
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
    auto mutable_attributes = node->attributes;
    bool needsId = false;
    for (const auto& child : node->children) {
        if (dynamic_cast<const ScriptNode*>(child.get())) {
            needsId = true;
            break;
        }
    }
    if (needsId && mutable_attributes.find("id") == mutable_attributes.end()) {
        mutable_attributes["id"] = generateUniqueId();
    }

    output << "<" << node->tagName;
    for (const auto& attr : mutable_attributes) {
        output << " " << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
    }

    std::stringstream inlineStyle;
    PropertyMap localProps;
    ExpressionEvaluator evaluator;
    for (const auto& child : node->children) {
        if (const auto* style = dynamic_cast<const StyleNode*>(child.get())) {
            for (const auto& styleChild : style->children) {
                 if (const auto* decl = dynamic_cast<const DeclarationNode*>(styleChild.get())) {
                    bool isSimple = decl->valueTokens.size() == 1 || (decl->valueTokens.size() == 2 && decl->valueTokens[0].type == TokenType::NUMBER && decl->valueTokens[1].type == TokenType::IDENTIFIER);
                    if (isSimple) {
                        PropertyMap emptyContext;
                        localProps[decl->property] = evaluator.visit(ExpressionParser(decl->valueTokens).parse().get(), emptyContext);
                    }
                }
            }
            for (const auto& styleChild : style->children) {
                if (const auto* decl = dynamic_cast<const DeclarationNode*>(styleChild.get())) {
                    inlineStyle << decl->property << ": " << evaluator.evaluate(ExpressionParser(decl->valueTokens).parse().get(), localProps) << "; ";
                } else if (const auto* rule = dynamic_cast<const RuleNode*>(styleChild.get())) {
                    visitRuleNode(rule, context, node);
                } else if (const auto* usage = dynamic_cast<const TemplateUsageNode*>(styleChild.get())) {
                    inlineStyle << visitTemplateUsageNode(usage, context, node);
                }
            }
        }
    }
    std::string styleAttr = inlineStyle.str();
    if (!styleAttr.empty()) output << " style=\"" << escapeHTML(styleAttr) << "\"";
    output << ">";

    for (const auto& child : node->children) {
        if (const auto* script = dynamic_cast<const ScriptNode*>(child.get())) {
            std::string rawScript = context.scriptBlocks.at(script->placeholder);
            CHTLJSParser parser(rawScript);
            auto cjsAst = parser.parse();
            CHTLJSGenerator cjsGen;
            std::string parentSelector = "#" + mutable_attributes.at("id");
            std::string processedJs = cjsGen.generate(cjsAst, parentSelector);
            context.globalJs << processedJs;
        } else if (!dynamic_cast<const StyleNode*>(child.get())) {
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
    PropertyMap emptyContext;
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
