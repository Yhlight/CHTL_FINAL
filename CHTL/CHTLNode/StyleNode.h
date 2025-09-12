#ifndef CHTL_STYLENODE_H
#define CHTL_STYLENODE_H

#include "BaseNode.h"
#include "../CHTLExpression/ExprNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL
{
    struct CssRule
    {
        std::string selector;
        std::map<std::string, std::unique_ptr<ExprNode>> properties;
    };

    class StyleNode : public BaseNode
    {
    public:
        // For inline styles, e.g. `style { color: red; }`
        std::map<std::string, std::unique_ptr<ExprNode>> inline_properties;

        // For rules that go into a global style sheet, e.g. `style { .box { ... } }`
        std::vector<CssRule> rules;

        void addInlineProperty(const std::string& key, std::unique_ptr<ExprNode> value);
        void addRule(CssRule&& rule);

        NodeType getType() const override { return NodeType::STYLE_NODE; }
        std::string toString(int indent = 0) const override;
    };
}

#endif // CHTL_STYLENODE_H
