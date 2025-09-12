#include "CHTLGenerator.h"
#include "AttributeNode.h"
#include <stdexcept>
#include <queue>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<ElementNode> root) : root(root) {}

std::string CHTLGenerator::generate() {
    // Before generating the string, modify the AST to include the global styles.
    injectGlobalStyles();

    // The root node from the parser is a synthetic "root" element.
    // We should generate its children directly without generating the "root" tag.
    for (const auto& child : root->children) {
        generateNode(child);
    }
    return output.str();
}

void CHTLGenerator::injectGlobalStyles() {
    if (root->global_styles.empty()) {
        return; // Nothing to do
    }

    // 1. Find the <head> node in the AST.
    auto headNode = findFirstNodeByTag("head", root);
    if (!headNode) {
        // No head tag found. For now, we'll just ignore the global styles.
        // A more advanced implementation might create the head and html tags.
        return;
    }

    // 2. Compile the global styles into a single string.
    std::stringstream style_content_ss;
    for (const auto& style_pair : root->global_styles) {
        style_content_ss << style_pair.first << " {\n" << style_pair.second << "}\n";
    }

    // 3. Create a new <style> element and inject it into the <head>.
    auto styleElement = std::make_shared<ElementNode>("style");
    auto styleText = std::make_shared<TextNode>("\n" + style_content_ss.str());
    styleElement->addChild(styleText);

    // Add the new <style> element to the beginning of the <head>'s children.
    headNode->children.insert(headNode->children.begin(), styleElement);
}

// Finds the first node with a given tag name using Breadth-First Search (BFS).
std::shared_ptr<ElementNode> CHTLGenerator::findFirstNodeByTag(const std::string& tagName, std::shared_ptr<ElementNode> startNode) {
    std::queue<std::shared_ptr<ElementNode>> q;
    q.push(startNode);

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        if (current->tagName == tagName) {
            return current;
        }

        for (const auto& childBase : current->children) {
            if (auto childElement = std::dynamic_pointer_cast<ElementNode>(childBase)) {
                q.push(childElement);
            }
        }
    }
    return nullptr; // Not found
}


void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // Using 2 spaces for indentation
    }
}

void CHTLGenerator::generateNode(const std::shared_ptr<BaseNode>& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        generateElement(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        generateText(text);
    }
}

void CHTLGenerator::generateElement(const std::shared_ptr<ElementNode>& node) {
    indent();
    output << "<" << node->tagName;

    // Add regular attributes
    for (const auto& attr : node->attributes) {
        output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    // Add style attribute from parsed styles
    if (!node->styles.empty()) {
        output << " style=\"";
        bool first = true;
        for (const auto& style_pair : node->styles) {
            if (!first) {
                output << " ";
            }
            output << style_pair.first << ": " << style_pair.second << ";";
            first = false;
        }
        output << "\"";
    }

    if (node->children.empty()) {
        output << "></" << node->tagName << ">\n";
    } else {
        output << ">\n";
        indentLevel++;
        for (const auto& child : node->children) {
            generateNode(child);
        }
        indentLevel--;
        indent();
        output << "</" << node->tagName << ">\n";
    }
}

void CHTLGenerator::generateText(const std::shared_ptr<TextNode>& node) {
    // For style tags, we don't want to indent the content.
    // A better check would be needed, but for now this is a simple heuristic.
    if (node->content.find(";") != std::string::npos) {
         output << node->content;
    } else {
        indent();
        output << node->content << "\n";
    }
}

} // namespace CHTL
