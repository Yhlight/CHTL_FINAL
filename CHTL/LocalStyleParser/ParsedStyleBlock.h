#ifndef CHTL_PARSED_STYLE_BLOCK_H
#define CHTL_PARSED_STYLE_BLOCK_H

#include "../CssValueParser/ValueAst.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

// Represents a CSS rule to be hoisted to a global <style> tag.
struct GlobalCssRule {
    std::string selector;
    std::string rawContent; // The raw content inside the { ... }
};

// Holds the complete parsed result of a local style {} block.
struct ParsedStyleBlock {
    // Properties to be applied as inline styles, stored as expression trees.
    std::map<std::string, std::shared_ptr<ExprNode>> inlineStyleTrees;
    // Full CSS rules to be hoisted to a global style tag.
    std::vector<GlobalCssRule> globalRules;
    // Names of templates being used, e.g. @Style MyTemplate;
    std::vector<std::string> templateUsages;
};

#endif // CHTL_PARSED_STYLE_BLOCK_H
