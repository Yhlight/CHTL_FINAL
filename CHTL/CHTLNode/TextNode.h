#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    std::string text;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_TEXT_NODE_H
