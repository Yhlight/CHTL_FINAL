#ifndef CHTL_STYLE_TEMPLATE_NODE_H
#define CHTL_STYLE_TEMPLATE_NODE_H

#include "BaseNode.h"
#include "../CssValueParser/ValueAst.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class StyleTemplateNode : public BaseNode {
public:
    std::string name;
    // The names of other style templates this one inherits from
    std::vector<std::string> baseTemplates;
    // The properties defined directly in this template
    std::map<std::string, std::shared_ptr<ExprNode>> properties;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_STYLE_TEMPLATE_NODE_H
