#include "Generator.h"
#include <stdexcept>

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

    // Add global styles before </head>
    std::string globalStyles = context.globalCss.str();
    if (!globalStyles.empty()) {
        std::string styleBlock = "<style>" + globalStyles + "</style>";
        size_t headEndPos = finalHtml.find("</head>");
        if (headEndPos != std::string::npos) finalHtml.insert(headEndPos, styleBlock);
        else finalHtml = styleBlock + finalHtml;
    }

    // Add global scripts before </body>
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
    else if (const auto* origin = dynamic_cast<const OriginNode*>(node)) visitOriginNode(origin, context);
    else if (const auto* usage = dynamic_cast<const OriginUsageNode*>(node)) visitOriginUsageNode(usage, context);
}

void Generator::visitElementNode(const ElementNode* node, CHTLContext& context) {
    output << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
    }
    output << ">";

    for (const auto& child : node->children) {
        visit(child.get(), context);
    }
    output << "</" << node->tagName << ">";
}

void Generator::visitTextNode(const TextNode* node, CHTLContext& context) {
    output << escapeHTML(node->content);
}

void Generator::visitOriginNode(const OriginNode* node, CHTLContext& context) {
    if (node->type == "Html") {
        output << node->content;
    } else if (node->type == "Style") {
        context.globalCss << node->content;
    } else if (node->type == "JavaScript") {
        context.globalJs << node->content;
    }
    // Silently ignore unknown origin types for now
}

void Generator::visitOriginUsageNode(const OriginUsageNode* node, CHTLContext& context) {
    if (context.namedOriginBlocks.count(node->name)) {
        const auto& originNode = context.namedOriginBlocks.at(node->name);
        // Once we have the named block, we just visit it like a normal origin block
        visitOriginNode(originNode.get(), context);
    }
    // Silently ignore if the named block is not found
}
