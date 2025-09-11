#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <string>

class StyleNode : public BaseNode {
public:
    // Holds the raw, unparsed content of the style block.
    std::string rawContent;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_STYLE_NODE_H
