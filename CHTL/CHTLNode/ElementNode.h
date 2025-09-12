#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "../CssValueParser/ValueAst.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> processedStyles;
    std::map<std::string, std::shared_ptr<ExprNode>> unevaluatedStyles;
    std::vector<std::shared_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_ELEMENT_NODE_H
