#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// A simple struct to hold a parsed CSS rule
struct CssRule {
    std::string selector;
    std::string body;
};

// The CHTLContext holds data for a single compilation run.
// This allows us to collect information from the entire file, like CSS rules
// that need to be hoisted to the top of the document.
class CHTLContext {
public:
    void addCssRule(const std::string& selector, const std::string& body) {
        cssRules.push_back({selector, body});
    }

    const std::vector<CssRule>& getCssRules() const {
        return cssRules;
    }

private:
    std::vector<CssRule> cssRules;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
