#include "CHTL/CHTLGenerator.h"
#include "CHTL/ElementNode.h"
#include "CHTL/TextNode.h"
#include "CHTL/StyleNode.h"
#include "CHTL/ScriptNode.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include "CHTL/NamespaceNode.h"
#include "CHTL/ConfigurationNode.h"
#include "CHTL/OriginNode.h"
#include "CHTL/OperatorNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : indentLevel_(0), indentSize_(2) {
    initializeGenerators();
    initializeCounters();
}

CHTLGenerator::~CHTLGenerator() = default;

void CHTLGenerator::initializeGenerators() {
    // Initialize HTML element generators
    elementGenerators_ = {
        {"html", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options);
        }},
        {"head", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"body", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"div", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"span", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"p", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"h1", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"h2", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"h3", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"h4", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"h5", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"h6", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"a", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"img", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"ul", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"ol", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"li", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"table", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"tr", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"td", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"th", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"form", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"input", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"button", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"textarea", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"select", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"option", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"nav", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"header", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"footer", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"main", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"section", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"article", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"aside", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"br", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"hr", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"strong", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"em", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"b", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"i", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"u", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"s", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"code", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }},
        {"pre", [this](const ElementNode& node, const GenerateOptions& options) {
            return generateHTML(node, options); // Placeholder
        }}
    };
    
    // Initialize CSS property generators
    cssGenerators_ = {
        {"color", [](const std::string& value) { return "color: " + value + ";"; }},
        {"background-color", [](const std::string& value) { return "background-color: " + value + ";"; }},
        {"width", [](const std::string& value) { return "width: " + value + ";"; }},
        {"height", [](const std::string& value) { return "height: " + value + ";"; }},
        {"margin", [](const std::string& value) { return "margin: " + value + ";"; }},
        {"padding", [](const std::string& value) { return "padding: " + value + ";"; }},
        {"border", [](const std::string& value) { return "border: " + value + ";"; }},
        {"font-size", [](const std::string& value) { return "font-size: " + value + ";"; }},
        {"font-family", [](const std::string& value) { return "font-family: " + value + ";"; }},
        {"text-align", [](const std::string& value) { return "text-align: " + value + ";"; }},
        {"line-height", [](const std::string& value) { return "line-height: " + value + ";"; }},
        {"display", [](const std::string& value) { return "display: " + value + ";"; }},
        {"position", [](const std::string& value) { return "position: " + value + ";"; }},
        {"top", [](const std::string& value) { return "top: " + value + ";"; }},
        {"left", [](const std::string& value) { return "left: " + value + ";"; }},
        {"right", [](const std::string& value) { return "right: " + value + ";"; }},
        {"bottom", [](const std::string& value) { return "bottom: " + value + ";"; }},
        {"z-index", [](const std::string& value) { return "z-index: " + value + ";"; }},
        {"opacity", [](const std::string& value) { return "opacity: " + value + ";"; }},
        {"visibility", [](const std::string& value) { return "visibility: " + value + ";"; }},
        {"overflow", [](const std::string& value) { return "overflow: " + value + ";"; }},
        {"float", [](const std::string& value) { return "float: " + value + ";"; }},
        {"clear", [](const std::string& value) { return "clear: " + value + ";"; }}
    };
}

void CHTLGenerator::initializeCounters() {
    elementCounter_ = 0;
    classCounter_ = 0;
    idCounter_ = 0;
    scriptCounter_ = 0;
    styleCounter_ = 0;
}

void CHTLGenerator::resetCounters() {
    initializeCounters();
}

CHTLGenerator::GenerateResult CHTLGenerator::generate(std::shared_ptr<BaseNode> ast, const GenerateOptions& options) {
    GenerateResult result;
    currentOptions_ = options;
    indentLevel_ = 0;
    
    try {
        // Generate HTML from the AST
        if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(ast)) {
            result.htmlOutput = generateHTML(*elementNode, options);
        } else {
            result.htmlOutput = "<div>Generated content</div>"; // Placeholder
        }
        
        result.cssOutput = generateCSS(ast, options);
        result.jsOutput = generateJS(ast, options);
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
    }
    
    return result;
}

std::string CHTLGenerator::generateHTML(const ElementNode& node, const GenerateOptions& options) {
    std::stringstream html;
    
    // Generate the element HTML
    html << "<" << node.getTagName();
    
    // Add attributes
    auto attributes = node.getAttributes();
    for (const auto& attr : attributes) {
        html << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    // Check if it's a self-closing tag
    if (node.isSelfClosing()) {
        html << " />";
    } else {
        html << ">";
        
        // Add children
        for (const auto& child : node.getChildren()) {
            if (auto childElement = std::dynamic_pointer_cast<ElementNode>(child)) {
                html << generateHTML(*childElement, options);
            } else if (auto childText = std::dynamic_pointer_cast<TextNode>(child)) {
                html << childText->getContent();
            }
            // Add other node types as needed
        }
        
        html << "</" << node.getTagName() << ">";
    }
    
    return html.str();
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> ast, const GenerateOptions& options) {
    std::stringstream css;
    
    if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(ast)) {
        // Generate CSS for element styles
        // Placeholder implementation
        css << "/* CSS placeholder */";
    } else if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(ast)) {
        // Generate CSS from style node
        css << generateStyleBlock(styleNode);
    }
    
    return css.str();
}

std::string CHTLGenerator::generateJS(std::shared_ptr<BaseNode> ast, const GenerateOptions& options) {
    std::stringstream js;
    
    if (auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(ast)) {
        // Generate JS from script node
        js << generateScriptBlock(scriptNode);
    }
    
    return js.str();
}

std::string CHTLGenerator::generateSourceMap(const std::string& originalCode, const std::string& generatedCode) {
    // Placeholder implementation
    return "/* Source map placeholder */";
}

std::string CHTLGenerator::generateElement(std::shared_ptr<ElementNode> element, int indent) {
    std::stringstream html;
    
    // Check if we have a custom generator for this element
    auto it = elementGenerators_.find(element->getTagName());
    if (it != elementGenerators_.end()) {
        return it->second(*element, currentOptions_);
    }
    
    // Default element generation
    html << "<" << element->getTagName();
    
    // Add attributes
    for (const auto& attr : element->getAttributes()) {
        html << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    // Check if it's a self-closing tag
    if (element->isSelfClosing()) {
        html << " />" << std::endl;
    } else {
        html << ">" << std::endl;
        
        // Add children
        indentLevel_++;
        for (const auto& child : element->getChildren()) {
            if (auto childElement = std::dynamic_pointer_cast<ElementNode>(child)) {
                html << generateElement(childElement, indent + 1);
            } else if (auto childText = std::dynamic_pointer_cast<TextNode>(child)) {
                html << childText->getContent();
            }
        }
        indentLevel_--;
        
        html << "</" << element->getTagName() << ">" << std::endl;
    }
    
    return html.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<TextNode> text, int indent) {
    return text->getContent();
}

std::string CHTLGenerator::generateAttributes(const std::map<std::string, std::string>& attributes) {
    std::stringstream result;
    for (const auto& attr : attributes) {
        result << " " << attr.first << "=\"" << attr.second << "\"";
    }
    return result.str();
}

std::string CHTLGenerator::generateAttribute(const std::string& name, const std::string& value) {
    return name + "=\"" + value + "\"";
}

std::string CHTLGenerator::generateStyleBlock(std::shared_ptr<StyleNode> style, int indent) {
    return "/* Style block placeholder */";
}

std::string CHTLGenerator::generateStyleProperties(const std::map<std::string, std::string>& properties, int indent) {
    return "/* Style properties placeholder */";
}

std::string CHTLGenerator::generateStyleProperty(const std::string& name, const std::string& value) {
    return name + ": " + value + ";";
}

std::string CHTLGenerator::generateClassSelector(const std::string& className, const std::map<std::string, std::string>& properties, int indent) {
    return "/* Class selector placeholder */";
}

std::string CHTLGenerator::generateIdSelector(const std::string& id, const std::map<std::string, std::string>& properties, int indent) {
    return "/* ID selector placeholder */";
}

std::string CHTLGenerator::generatePseudoSelector(const std::string& pseudo, const std::map<std::string, std::string>& properties, int indent) {
    return "/* Pseudo selector placeholder */";
}

std::string CHTLGenerator::generateMediaQuery(const std::string& media, const std::map<std::string, std::string>& properties, int indent) {
    return "/* Media query placeholder */";
}

std::string CHTLGenerator::generateScriptBlock(std::shared_ptr<ScriptNode> script, int indent) {
    return "/* Script block placeholder */";
}

std::string CHTLGenerator::generateScriptContent(const std::string& content, int indent) {
    return content;
}

std::string CHTLGenerator::generateEventHandlers(const std::map<std::string, std::string>& handlers, int indent) {
    return "/* Event handlers placeholder */";
}

std::string CHTLGenerator::generateVirtualObjects(const std::map<std::string, std::string>& objects, int indent) {
    return "/* Virtual objects placeholder */";
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<TemplateNode> templateNode, int indent) {
    return "/* Template placeholder */";
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<CustomNode> customNode, int indent) {
    return "/* Custom placeholder */";
}

std::string CHTLGenerator::generateImport(std::shared_ptr<ImportNode> importNode, int indent) {
    return "/* Import placeholder */";
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<NamespaceNode> namespaceNode, int indent) {
    return "/* Namespace placeholder */";
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<ConfigurationNode> config, int indent) {
    return "/* Configuration placeholder */";
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<OriginNode> origin, int indent) {
    return "/* Origin placeholder */";
}

std::string CHTLGenerator::generateOperator(std::shared_ptr<OperatorNode> op, int indent) {
    return "/* Operator placeholder */";
}

// Placeholder implementations for remaining methods
std::string CHTLGenerator::generateDefaultHTMLStructure(const std::string& content, const GenerateOptions& options) {
    return "<!DOCTYPE html>\n<html>\n<head></head>\n<body>" + content + "</body>\n</html>";
}

std::string CHTLGenerator::generateDefaultCSSStructure(const std::string& content, const GenerateOptions& options) {
    return "/* CSS Structure */\n" + content;
}

std::string CHTLGenerator::generateDefaultJSStructure(const std::string& content, const GenerateOptions& options) {
    return "// JS Structure\n" + content;
}

void CHTLGenerator::setContext(std::shared_ptr<void> context) {
    context_ = context;
}

std::shared_ptr<void> CHTLGenerator::getContext() const {
    return context_;
}

void CHTLGenerator::reset() {
    context_.reset();
    currentOptions_ = GenerateOptions();
    indentLevel_ = 0;
    resetCounters();
}

} // namespace CHTL