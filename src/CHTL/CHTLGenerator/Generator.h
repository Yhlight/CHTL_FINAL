#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <string>
#include <sstream>

class Generator {
public:
    std::string generate(const NodeList& nodes);

private:
    void generateNode(const BaseNode* node);
    void generateElement(const ElementNode* node);
    void generateText(const TextNode* node);
    void generateComment(const CommentNode* node);

    std::stringstream output;
};
