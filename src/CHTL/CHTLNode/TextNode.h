#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    TextNode(const std::string& text);

    void accept(Visitor& visitor) override;

    const std::string& getText() const;

private:
    std::string text;
};

#endif // TEXT_NODE_H
