#include "Generator.h"
#include <stdexcept>
#include <set>
#include <map>
#include <functional>
// Set of HTML5 void elements. These tags cannot have any content.
static const std::set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator(std::shared_ptr<Program> program, Context& context)
    : program(std::move(program)), context(context), indentLevel(0) {}

void Generator::generate() {
    if (program) {
        generateProgram(program);
    }
}

std::string Generator::getHtml() const {
    return htmlStream.str();
}

std::string Generator::getCss() const {
    return cssStream.str();
}

void Generator::writeIndent() {
    for (int i = 0; i < indentLevel; ++i) {
        htmlStream << "  "; // 2 spaces for indentation
    }
}

// Main dispatcher function
void Generator::generateNode(const std::shared_ptr<Node>& node) {
    if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        generateElement(elementNode);
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        generateText(textNode);
    } else if (auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
        // Handle @Element MyTemplate; usage
        if (usageNode->token.literal == "Element") {
            auto elementTemplate = context.getElementTemplate(usageNode->name);
            if (elementTemplate) {
                for (const auto& child : elementTemplate->children) {
                    generateNode(child); // Recursively generate the template's content
                }
            }
        }
        // @Style usage is handled inside generateStyle, so we ignore it here.
    } else if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(node)) {
        // Style nodes are handled within generateElement, so we can ignore them here
        // to avoid them being processed twice.
    } else if (auto commentNode = std::dynamic_pointer_cast<CommentNode>(node)) {
        generateComment(commentNode);
    }
    // Other node types would be dispatched here...
}

void Generator::generateProgram(const std::shared_ptr<Program>& node) {
    for (const auto& stmt : node->statements) {
        generateNode(stmt);
    }
}

// Forward declaration for the recursive helper
std::map<std::string, std::string> computeStyleProperties(const std::vector<std::shared_ptr<Statement>>& statements, Context& context, std::function<std::string(std::shared_ptr<Expression>)> expGen);

std::map<std::string, std::string> computeUsage(const std::shared_ptr<TemplateUsageNode>& usage, Context& context, std::function<std::string(std::shared_ptr<Expression>)> expGen) {
    if (usage->token.literal != "Style") return {};

    auto styleCustom = context.getStyleCustom(usage->name);
    if (styleCustom) {
        // It's a custom style. Recursively compute its base properties.
        std::map<std::string, std::string> props = computeStyleProperties(styleCustom->statements, context, expGen);
        // Now, apply specializations from the usage site.
        for (const auto& specStmt : usage->specializations) {
            if (auto attr = std::dynamic_pointer_cast<AttributeNode>(specStmt)) {
                props[attr->key] = expGen(attr->value); // Add or overwrite
            } else if (auto del = std::dynamic_pointer_cast<DeletePropertyNode>(specStmt)) {
                for (const auto& propName : del->propertiesToDelete) {
                    props.erase(propName);
                }
            }
        }
        return props;
    } else {
        // It's a simple template.
        auto styleTemplate = context.getStyleTemplate(usage->name);
        if (styleTemplate) {
            std::map<std::string, std::string> props;
            for (const auto& prop : styleTemplate->properties) {
                props[prop->key] = expGen(prop->value);
            }
            return props;
        }
    }
    return {};
}

std::map<std::string, std::string> computeStyleProperties(const std::vector<std::shared_ptr<Statement>>& statements, Context& context, std::function<std::string(std::shared_ptr<Expression>)> expGen) {
    std::map<std::string, std::string> props;
    for(const auto& stmt : statements) {
        if (auto rule = std::dynamic_pointer_cast<CssRuleNode>(stmt)) {
            if (rule->selector == "&") {
                for (const auto& prop : rule->properties) {
                    props[prop->key] = expGen(prop->value);
                }
            }
        } else if (auto attr = std::dynamic_pointer_cast<AttributeNode>(stmt)) {
            props[attr->key] = attr->value ? expGen(attr->value) : "";
        } else if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(stmt)) {
            auto resolvedProps = computeUsage(usage, context, expGen);
            for (const auto& [key, val] : resolvedProps) {
                props[key] = val;
            }
        }
    }
    return props;
}

void Generator::generateStyle(const std::shared_ptr<StyleNode>& styleNode, std::map<std::string, std::string>& attributes) {
    std::string mainSelector;

    auto expGen = [this](std::shared_ptr<Expression> node) {
        return this->generateExpression(node);
    };

    // First pass: Compute all final inline styles by processing all statements.
    std::map<std::string, std::string> finalInlineStyles = computeStyleProperties(styleNode->statements, context, expGen);

    for(const auto& [key, val] : finalInlineStyles) {
        if (!val.empty()) {
            attributes["style"] += key + ":" + val + ";";
        }
    }

    // Second pass: find main selector for global rules and add classes/ids
    for (const auto& stmt : styleNode->statements) {
         if (auto rule = std::dynamic_pointer_cast<CssRuleNode>(stmt)) {
            if (mainSelector.empty()) {
                if (rule->selector[0] == '.') {
                    attributes["class"] = attributes["class"].empty() ? rule->selector.substr(1) : attributes["class"] + " " + rule->selector.substr(1);
                    mainSelector = rule->selector;
                } else if (rule->selector[0] == '#') {
                    attributes["id"] = rule->selector.substr(1);
                    mainSelector = rule->selector;
                }
            }
         }
    }
    if (mainSelector.empty()) {
        if (attributes.count("id")) mainSelector = "#" + attributes["id"];
        else if (attributes.count("class")) mainSelector = "." + attributes["class"].substr(0, attributes["class"].find(' '));
    }

    // Third pass: generate global CSS for all nested rules
    for (const auto& stmt : styleNode->statements) {
        if (auto rule = std::dynamic_pointer_cast<CssRuleNode>(stmt)) {
            if (rule->selector == "&") continue; // Skip inline styles

            std::string finalSelector = rule->selector;
            if (finalSelector[0] == '&') {
                finalSelector.replace(0, 1, mainSelector);
            }

            cssStream << finalSelector << " {\n";
            for (const auto& prop : rule->properties) {
                cssStream << "  " << prop->key << ": " << generateExpression(prop->value) << ";\n";
            }
            cssStream << "}\n\n";
        }
    }
}


void Generator::generateElement(const std::shared_ptr<ElementNode>& node) {
    // Use a map to easily manage attributes
    std::map<std::string, std::string> attributes;
    for (const auto& attr : node->attributes) {
        attributes[attr->key] = generateExpression(attr->value);
    }

    // Find and process the style node, which might modify the attributes map
    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            generateStyle(styleNode, attributes);
            break; // Assume only one style block per element
        }
    }

    writeIndent();
    htmlStream << "<" << node->tagName;
    for (const auto& [key, val] : attributes) {
        htmlStream << " " << key << "=\"" << val << "\"";
    }

    // Handle tag closing
    if (voidElements.count(node->tagName)) {
        htmlStream << ">\n";
        return;
    }

    htmlStream << ">\n";

    bool hasRenderableChildren = false;
    for (const auto& child : node->children) {
        if (!std::dynamic_pointer_cast<StyleNode>(child)) {
            hasRenderableChildren = true;
            break;
        }
    }

    if (hasRenderableChildren) {
        indentLevel++;
        for (const auto& child : node->children) {
            generateNode(child);
        }
        indentLevel--;
        writeIndent();
    }

    htmlStream << "</" << node->tagName << ">\n";
}

void Generator::generateText(const std::shared_ptr<TextNode>& node) {
    writeIndent();
    htmlStream << node->content << "\n";
}

void Generator::generateComment(const std::shared_ptr<CommentNode>& node) {
    writeIndent();
    // The literal includes the '# ', so we skip it.
    htmlStream << "<!--" << node->content.substr(2) << "-->\n";
}

std::string Generator::generateExpression(const std::shared_ptr<Expression>& node) {
    if (auto identNode = std::dynamic_pointer_cast<IdentifierNode>(node)) {
        return identNode->value;
    } else if (auto strNode = std::dynamic_pointer_cast<StringLiteralNode>(node)) {
        return strNode->value;
    } else if (auto varUsageNode = std::dynamic_pointer_cast<VarUsageNode>(node)) {
        auto varTemplate = context.getVarTemplate(varUsageNode->groupName);
        if (varTemplate) {
            for (const auto& var : varTemplate->variables) {
                if (var->key == varUsageNode->variableName) {
                    // Recursively call generateExpression to resolve the value
                    return generateExpression(var->value);
                }
            }
        }
    }
    return "";
}
