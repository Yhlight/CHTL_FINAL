#include "Generator.h"
#include <unordered_set>

namespace CHTL {

// Set of HTML tags that are self-closing
const std::unordered_set<std::string> self_closing_tags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator() : m_indentLevel(0) {}

std::string Generator::getIndent() {
    std::string indent = "";
    for (int i = 0; i < m_indentLevel; ++i) {
        indent += m_indentStr;
    }
    return indent;
}

void Generator::write(const std::string& content) {
    m_out << content;
}

void Generator::writeLine(const std::string& line) {
    m_out << getIndent() << line << "\n";
}

std::string Generator::generate(const ProgramNode& program) {
    for (const auto& child : program.children) {
        generateNode(child);
    }
    return m_out.str();
}

void Generator::generateNode(const std::shared_ptr<Node>& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        generateElement(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        generateText(text);
    } else if (auto comment = std::dynamic_pointer_cast<CommentNode>(node)) {
        generateComment(comment);
    }
}

void Generator::generateElement(const std::shared_ptr<ElementNode>& node) {
    // Start opening tag
    write(getIndent() + "<" + node->tagName);

    // Add attributes
    for (const auto& attr : node->attributes) {
        write(" " + attr->name + "=\"" + attr->value + "\"");
    }

    bool is_self_closing = self_closing_tags.count(node->tagName) > 0;
    if (is_self_closing) {
        write(">\n");
        return;
    }

    write(">\n");

    // Children
    m_indentLevel++;
    for (const auto& child : node->children) {
        generateNode(child);
    }
    m_indentLevel--;

    // Closing tag
    writeLine("</" + node->tagName + ">");
}

void Generator::generateText(const std::shared_ptr<TextNode>& node) {
    m_indentLevel++;
    writeLine(node->content);
    m_indentLevel--;
}

void Generator::generateComment(const std::shared_ptr<CommentNode>& node) {
    writeLine("<!--" + node->text + " -->");
}

} // namespace CHTL
