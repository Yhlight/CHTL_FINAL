#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : format_output(true), indent_level(0), indent_string("  ") {
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<RootNode> ast) {
    if (!ast) {
        return "";
    }
    
    return generateHTMLDocument(ast);
}

std::string CHTLGenerator::generateHTMLDocument(std::shared_ptr<RootNode> ast) {
    std::ostringstream html;
    
    // Generate DOCTYPE and HTML structure
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    
    // Generate head
    html << generateHTMLHead();
    
    // Generate body content
    std::string body_content = generateElementContent(ast->root_content);
    html << generateHTMLBody(body_content);
    
    html << "</html>\n";
    
    return format_output ? formatHTML(html.str()) : html.str();
}

std::string CHTLGenerator::generateHTMLHead() {
    std::ostringstream head;
    head << getIndent() << "<head>\n";
    increaseIndent();
    
    head << getIndent() << "<meta charset=\"UTF-8\">\n";
    head << getIndent() << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    head << getIndent() << "<title>CHTL Generated Page</title>\n";
    
    // Add any CSS or JS imports here
    // This will be enhanced when we implement the import system
    
    decreaseIndent();
    head << getIndent() << "</head>\n";
    
    return head.str();
}

std::string CHTLGenerator::generateHTMLBody(const std::string& content) {
    std::ostringstream body;
    body << getIndent() << "<body>\n";
    increaseIndent();
    
    if (format_output) {
        body << content;
    } else {
        body << content;
    }
    
    decreaseIndent();
    body << getIndent() << "</body>\n";
    
    return body.str();
}

std::string CHTLGenerator::generateTextNode(std::shared_ptr<TextNode> node) {
    if (!node) return "";
    return escapeHTML(node->value);
}

std::string CHTLGenerator::generateElementNode(std::shared_ptr<ElementNode> node) {
    if (!node) return "";
    
    std::ostringstream element;
    
    // Generate opening tag
    element << getIndent() << "<" << node->tag_name;
    
    // Generate attributes
    std::string attributes = generateAttributes(node->attributes);
    if (!attributes.empty()) {
        element << " " << attributes;
    }
    
    // Check if element has content
    if (node->content.empty()) {
        element << " />";
        if (format_output) element << "\n";
    } else {
        element << ">";
        if (format_output) element << "\n";
        
        // Generate content
        increaseIndent();
        element << generateElementContent(node->content);
        decreaseIndent();
        
        element << getIndent() << "</" << node->tag_name << ">";
        if (format_output) element << "\n";
    }
    
    return element.str();
}

std::string CHTLGenerator::generateStyleNode(std::shared_ptr<StyleNode> node) {
    if (!node) return "";
    
    std::ostringstream style;
    style << getIndent() << "<style>\n";
    increaseIndent();
    
    // Generate CSS content
    std::string css_content = generateCSS(node);
    style << css_content;
    
    decreaseIndent();
    style << getIndent() << "</style>\n";
    
    return style.str();
}

std::string CHTLGenerator::generateScriptNode(std::shared_ptr<ScriptNode> node) {
    if (!node) return "";
    
    std::ostringstream script;
    script << getIndent() << "<script>\n";
    increaseIndent();
    
    // Generate JavaScript content
    std::string js_content = generateJavaScript(node);
    script << js_content;
    
    decreaseIndent();
    script << getIndent() << "</script>\n";
    
    return script.str();
}

std::string CHTLGenerator::generateTemplateNode(std::shared_ptr<TemplateNode> node) {
    if (!node) return "";
    
    // Register template for later use
    registerTemplate(node);
    
    // Templates don't generate HTML directly, they're used for references
    return "";
}

std::string CHTLGenerator::generateCustomNode(std::shared_ptr<CustomNode> node) {
    if (!node) return "";
    
    // Register custom for later use
    registerCustom(node);
    
    // Custom nodes don't generate HTML directly, they're used for references
    return "";
}

std::string CHTLGenerator::generateImportNode(std::shared_ptr<ImportNode> node) {
    if (!node) return "";
    
    if (node->import_type == "CSS") {
        return getIndent() + "<link rel=\"stylesheet\" href=\"" + node->import_path + "\">\n";
    } else if (node->import_type == "JS") {
        return getIndent() + "<script src=\"" + node->import_path + "\"></script>\n";
    } else if (node->import_type == "HTML") {
        return getIndent() + "<!-- Import: " + node->import_path + " -->\n";
    }
    
    return "";
}

std::string CHTLGenerator::generateNamespaceNode(std::shared_ptr<NamespaceNode> node) {
    if (!node) return "";
    
    // Namespaces don't generate HTML directly
    return "";
}

std::string CHTLGenerator::generateConfigurationNode(std::shared_ptr<ConfigurationNode> node) {
    if (!node) return "";
    
    // Configuration nodes don't generate HTML directly
    return "";
}

std::string CHTLGenerator::generateUseNode(std::shared_ptr<UseNode> node) {
    if (!node) return "";
    
    if (node->use_target == "html5") {
        return "<!DOCTYPE html>\n";
    }
    
    return "";
}

std::string CHTLGenerator::generateOriginNode(std::shared_ptr<OriginNode> node) {
    if (!node) return "";
    
    return node->origin_content;
}

void CHTLGenerator::registerTemplate(std::shared_ptr<TemplateNode> template_node) {
    if (template_node) {
        templates[template_node->template_name] = template_node;
    }
}

void CHTLGenerator::registerCustom(std::shared_ptr<CustomNode> custom_node) {
    if (custom_node) {
        customs[custom_node->custom_name] = custom_node;
    }
}

std::shared_ptr<TemplateNode> CHTLGenerator::resolveTemplate(const std::string& name) {
    auto it = templates.find(name);
    return (it != templates.end()) ? it->second : nullptr;
}

std::shared_ptr<CustomNode> CHTLGenerator::resolveCustom(const std::string& name) {
    auto it = customs.find(name);
    return (it != customs.end()) ? it->second : nullptr;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<StyleNode> style_node) {
    if (!style_node) return "";
    
    std::ostringstream css;
    
    for (const auto& rule : style_node->style_rules) {
        css << getIndent() << rule->toHTML() << "\n";
    }
    
    return css.str();
}

std::string CHTLGenerator::generateCSSRules(const std::vector<std::shared_ptr<CHTLNode>>& rules) {
    std::ostringstream css;
    
    for (const auto& rule : rules) {
        css << getIndent() << rule->toHTML() << "\n";
    }
    
    return css.str();
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<ScriptNode> script_node) {
    if (!script_node) return "";
    
    std::ostringstream js;
    
    for (const auto& content : script_node->script_content) {
        js << getIndent() << content->toHTML() << "\n";
    }
    
    return js.str();
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string escaped = text;
    
    // Replace HTML special characters using a loop to avoid character literal issues
    for (size_t i = 0; i < escaped.length(); ++i) {
        switch (escaped[i]) {
            case '&':
                escaped.replace(i, 1, "&amp;");
                i += 4; // Skip the replaced characters
                break;
            case '<':
                escaped.replace(i, 1, "&lt;");
                i += 3;
                break;
            case '>':
                escaped.replace(i, 1, "&gt;");
                i += 3;
                break;
            case '"':
                escaped.replace(i, 1, "&quot;");
                i += 5;
                break;
            case '\'':
                escaped.replace(i, 1, "&#39;");
                i += 4;
                break;
        }
    }
    
    return escaped;
}

std::string CHTLGenerator::formatHTML(const std::string& html) {
    // Simple HTML formatting - can be enhanced later
    std::ostringstream formatted;
    std::istringstream input(html);
    std::string line;
    
    while (std::getline(input, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (!line.empty()) {
            formatted << line << "\n";
        }
    }
    
    return formatted.str();
}

bool CHTLGenerator::shouldFormatOutput() const {
    return format_output;
}

void CHTLGenerator::setFormatOutput(bool format) {
    format_output = format;
}

std::string CHTLGenerator::getIndent() {
    return std::string(indent_level * indent_string.length(), ' ');
}

void CHTLGenerator::increaseIndent() {
    indent_level++;
}

void CHTLGenerator::decreaseIndent() {
    if (indent_level > 0) {
        indent_level--;
    }
}

std::string CHTLGenerator::generateAttributes(const std::vector<std::shared_ptr<AttributeNode>>& attributes) {
    std::ostringstream attrs;
    
    for (size_t i = 0; i < attributes.size(); ++i) {
        if (i > 0) attrs << " ";
        
        const auto& attr = attributes[i];
        
        // Handle special reference attributes
        if (attr->attribute_name.substr(0, 1) == "@") {
            // This is a template or custom reference
            std::string ref_type = attr->attribute_name.substr(1);
            std::string ref_name = attr->attribute_value;
            
            // Resolve the reference
            std::string resolved = resolveElementReference(ref_type, ref_name);
            if (!resolved.empty()) {
                attrs << resolved;
            }
        } else {
            // Regular attribute
            attrs << attr->attribute_name;
            if (!attr->attribute_value.empty()) {
                if (attr->is_quoted) {
                    attrs << "=\"" << attr->attribute_value << "\"";
                } else {
                    attrs << "=" << attr->attribute_value;
                }
            }
        }
    }
    
    return attrs.str();
}

std::string CHTLGenerator::generateElementContent(const std::vector<std::shared_ptr<CHTLNode>>& content) {
    std::ostringstream content_str;
    
    for (const auto& node : content) {
        if (!node) continue;
        
        switch (node->node_type) {
            case NodeType::Text:
                content_str << generateTextNode(std::static_pointer_cast<TextNode>(node));
                break;
            case NodeType::Element:
                content_str << generateElementNode(std::static_pointer_cast<ElementNode>(node));
                break;
            case NodeType::Style:
                content_str << generateStyleNode(std::static_pointer_cast<StyleNode>(node));
                break;
            case NodeType::Script:
                content_str << generateScriptNode(std::static_pointer_cast<ScriptNode>(node));
                break;
            case NodeType::Template:
                content_str << generateTemplateNode(std::static_pointer_cast<TemplateNode>(node));
                break;
            case NodeType::Custom:
                content_str << generateCustomNode(std::static_pointer_cast<CustomNode>(node));
                break;
            case NodeType::Import:
                content_str << generateImportNode(std::static_pointer_cast<ImportNode>(node));
                break;
            case NodeType::Namespace:
                content_str << generateNamespaceNode(std::static_pointer_cast<NamespaceNode>(node));
                break;
            case NodeType::Configuration:
                content_str << generateConfigurationNode(std::static_pointer_cast<ConfigurationNode>(node));
                break;
            case NodeType::Use:
                content_str << generateUseNode(std::static_pointer_cast<UseNode>(node));
                break;
            case NodeType::Origin:
                content_str << generateOriginNode(std::static_pointer_cast<OriginNode>(node));
                break;
            default:
                // Unknown node type, try to generate as text
                content_str << node->toHTML();
                break;
        }
    }
    
    return content_str.str();
}

std::string CHTLGenerator::resolveElementReference(const std::string& ref_type, const std::string& ref_name) {
    if (ref_type == "Element") {
        auto custom = resolveCustom(ref_name);
        if (custom) {
            return generateCustomContent(custom);
        }
    } else if (ref_type == "Style") {
        auto template_node = resolveTemplate(ref_name);
        if (template_node) {
            return generateTemplateContent(template_node);
        }
    }
    
    return "";
}

std::string CHTLGenerator::generateTemplateContent(std::shared_ptr<TemplateNode> template_node) {
    if (!template_node) return "";
    
    std::ostringstream content;
    
    for (const auto& node : template_node->template_content) {
        content << node->toHTML() << "\n";
    }
    
    return content.str();
}

std::string CHTLGenerator::generateCustomContent(std::shared_ptr<CustomNode> custom_node) {
    if (!custom_node) return "";
    
    std::ostringstream content;
    
    for (const auto& node : custom_node->custom_content) {
        switch (node->node_type) {
            case NodeType::Element:
                content << generateElementNode(std::static_pointer_cast<ElementNode>(node));
                break;
            case NodeType::Style:
                content << generateStyleNode(std::static_pointer_cast<StyleNode>(node));
                break;
            case NodeType::Script:
                content << generateScriptNode(std::static_pointer_cast<ScriptNode>(node));
                break;
            default:
                content << node->toHTML();
                break;
        }
    }
    
    return content.str();
}

} // namespace CHTL