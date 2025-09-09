#include "BaseNode.h"
#include "ElementNode.h"
#include "StyleNode.h"
#include "TemplateNode.h"
#include "ModuleNode.h"
#include "RawBlockNode.h"

#include <string>

namespace CHTL {

// BaseNode.h
std::string Program::toString() const {
    std::string out;
    for (const auto& stmt : statements) {
        out += stmt->toString();
    }
    return out;
}

// ElementNode.h
std::string AttributeNode::toString() const { return key.literal + "=" + value->toString(); }
std::string TextNode::toString() const { return value; }
std::string CommentNode::toString() const { return value; }
std::string ElementNode::toString() const { return token.literal; }

// StyleNode.h
std::string StyleRuleNode::toString() const { return selector.literal + " { ... }"; }
std::string StylePropertyNode::toString() const { return key.literal + ": ...;"; }
std::string StyleNode::toString() const { return "style { ... }"; }

// TemplateNode.h
std::string TemplateNode::toString() const {
    return "[Template] " + type.literal + " " + name.literal + " { ... }";
}
std::string TemplateInstantiationNode::toString() const {
    return token.literal + " " + name.literal + ";";
}

// ModuleNode.h
std::string NamespaceNode::toString() const {
    return "[Namespace] " + name.literal + " { ... }";
}
std::string ImportNode::toString() const {
    return "[Import] ... from " + path;
}

// RawBlockNode.h
std::string RawBlockNode::toString() const {
    return token.literal + " { ... }";
}

} // namespace CHTL
