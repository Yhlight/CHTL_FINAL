#include "StatementNodes.h"
#include <string>

// Returns a string representation of an AttributeNode for debugging.
std::string AttributeNode::toString() const {
    std::string result = key;
    if (value) {
        result += ": " + value->toString();
    }
    result += ";";
    return result;
}

// Returns a string representation of an ElementNode for debugging.
std::string ElementNode::toString() const {
    std::string result = tagName + " {\n";
    for (const auto& attr : attributes) {
        if (attr) result += "\t" + attr->toString() + "\n";
    }
    for (const auto& child : children) {
        if (child) result += "\t" + child->toString() + "\n"; // Basic indentation
    }
    result += "}\n";
    return result;
}

// Returns a string representation of a CssRuleNode for debugging.
std::string CssRuleNode::toString() const {
    std::string out = selector + " {\n";
    for (const auto& prop : properties) {
        if (prop) out += "\t\t" + prop->toString() + "\n";
    }
    out += "\t}\n";
    return out;
}

// Returns a string representation of a StyleNode for debugging.
std::string StyleNode::toString() const {
    std::string result = "style {\n";
    for (const auto& rule : rules) {
        if (rule) result += "\t" + rule->toString();
    }
    result += "}\n";
    return result;
}
