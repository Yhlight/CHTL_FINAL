#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility>

namespace CHTL {

struct CssRule {
    std::string selector;
    std::vector<std::pair<std::string, std::string>> properties;
};

class StyleNode : public BaseNode {
public:
    StyleNode();

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    void addRule(const CssRule& rule);
    const std::vector<CssRule>& getRules() const;

private:
    std::vector<CssRule> m_rules;
};

} // namespace CHTL

#endif // STYLE_NODE_H
