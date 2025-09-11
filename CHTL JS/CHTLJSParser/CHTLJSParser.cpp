#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJS_TextNode.h"
#include "../CHTLJSNode/CHTLJS_SelectorNode.h"

CHTLJSParser::CHTLJSParser(const std::string& source) : source(source) {}

std::vector<std::unique_ptr<CHTLJS_BaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJS_BaseNode>> nodes;
    size_t lastPos = 0;
    size_t pos = 0;

    while ((pos = source.find("{{", lastPos)) != std::string::npos) {
        // Add the text before the selector as a TextNode
        if (pos > lastPos) {
            nodes.push_back(std::make_unique<CHTLJS_TextNode>(source.substr(lastPos, pos - lastPos)));
        }

        // Find the closing "}}"
        size_t endPos = source.find("}}", pos);
        if (endPos == std::string::npos) {
            // No closing tag, treat the rest as text
            break;
        }

        // Extract selector content and create a SelectorNode
        std::string selector = source.substr(pos + 2, endPos - pos - 2);
        nodes.push_back(std::make_unique<CHTLJS_SelectorNode>(selector));

        lastPos = endPos + 2;
    }

    // Add any remaining text after the last selector
    if (lastPos < source.length()) {
        nodes.push_back(std::make_unique<CHTLJS_TextNode>(source.substr(lastPos)));
    }

    return nodes;
}
