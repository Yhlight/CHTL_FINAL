#ifndef CHTLSTYLE_H
#define CHTLSTYLE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// CSS Rule types
enum class CSSRuleType {
    Class,
    Id,
    Element,
    PseudoClass,
    PseudoElement,
    Attribute,
    Universal,
    Descendant,
    Child,
    Adjacent,
    Sibling
};

// CSS Property types
enum class CSSPropertyType {
    Color,
    Background,
    Border,
    Layout,
    Typography,
    Spacing,
    Display,
    Position,
    Animation,
    Other
};

// CSS Rule structure
struct CSSRule {
    std::string selector;
    CSSRuleType rule_type;
    std::map<std::string, std::string> properties;
    size_t specificity;
    size_t line;
    size_t column;
    
    CSSRule() : rule_type(CSSRuleType::Element), specificity(0), line(0), column(0) {}
    CSSRule(const std::string& sel, CSSRuleType type, size_t l = 0, size_t c = 0)
        : selector(sel), rule_type(type), specificity(0), line(l), column(c) {}
};

// CSS Context for scoping
struct CSSContext {
    std::string parent_selector;
    std::vector<std::string> class_names;
    std::vector<std::string> id_names;
    std::string element_name;
    std::map<std::string, std::string> attributes;
    
    CSSContext() = default;
    CSSContext(const std::string& parent) : parent_selector(parent) {}
};

// Style block processor
class CHTLStyleProcessor {
public:
    CHTLStyleProcessor();
    ~CHTLStyleProcessor() = default;
    
    // Main processing methods
    std::vector<CSSRule> processStyleBlock(std::shared_ptr<StyleNode> style_node, const CSSContext& context = CSSContext());
    std::string generateCSS(const std::vector<CSSRule>& rules);
    
    // Context derivation
    CSSContext deriveContext(std::shared_ptr<ElementNode> element, const CSSContext& parent_context = CSSContext());
    std::string generateSelector(const CSSContext& context, const std::string& base_selector = "");
    
    // Auto class/ID generation
    std::string generateAutoClass(const std::string& element_name, const std::map<std::string, std::string>& attributes);
    std::string generateAutoId(const std::string& element_name, const std::map<std::string, std::string>& attributes);
    
    // Property condition expressions
    std::string evaluatePropertyCondition(const std::string& expression, const CSSContext& context);
    bool evaluateLogicalExpression(const std::string& expression, const CSSContext& context);
    
    // CSS parsing
    CSSRule parseCSSRule(const std::string& rule_text, const CSSContext& context = CSSContext());
    std::map<std::string, std::string> parseCSSProperties(const std::string& properties_text);
    
    // Specificity calculation
    size_t calculateSpecificity(const std::string& selector);
    size_t calculateSelectorSpecificity(const std::string& selector);
    
    // CSS optimization
    std::vector<CSSRule> optimizeCSS(const std::vector<CSSRule>& rules);
    std::vector<CSSRule> mergeDuplicateRules(const std::vector<CSSRule>& rules);
    
    // Context inference
    std::string inferContextSelector(const std::string& selector, const CSSContext& context);
    std::string resolveContextReference(const std::string& reference, const CSSContext& context);
    
    // Chained expressions
    std::string processChainedExpression(const std::string& expression, const CSSContext& context);
    std::string processOptionalExpression(const std::string& expression, const CSSContext& context);
    std::string processLogicalExpression(const std::string& expression, const CSSContext& context);
    
    // Pointing attribute expressions
    std::string processPointingAttribute(const std::string& expression, const CSSContext& context);
    std::string resolveAttributeReference(const std::string& reference, const CSSContext& context);
    
private:
    // Helper methods
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    bool isOperator(const std::string& token);
    bool isLogicalOperator(const std::string& token);
    std::string getOperatorPrecedence(const std::string& op);
    
    // CSS property validation
    bool isValidCSSProperty(const std::string& property);
    bool isValidCSSValue(const std::string& value);
    CSSPropertyType getPropertyType(const std::string& property);
    
    // Context management
    std::vector<CSSContext> context_stack;
    void pushContext(const CSSContext& context);
    void popContext();
    CSSContext getCurrentContext();
    
    // Auto-generation counters
    std::map<std::string, int> class_counters;
    std::map<std::string, int> id_counters;
    
    // Configuration
    bool auto_generate_classes;
    bool auto_generate_ids;
    bool enable_context_inference;
    std::string class_prefix;
    std::string id_prefix;
};

// Style system manager
class CHTLStyleManager {
public:
    CHTLStyleManager();
    ~CHTLStyleManager() = default;
    
    // Style processing
    void processDocument(std::shared_ptr<RootNode> ast);
    void processStyleBlocks(std::shared_ptr<CHTLNode> node);
    std::string generateDocumentCSS();
    
    // Style registration
    void registerStyleBlock(std::shared_ptr<StyleNode> style_node, const CSSContext& context);
    void registerTemplate(std::shared_ptr<TemplateNode> template_node);
    void registerCustom(std::shared_ptr<CustomNode> custom_node);
    
    // Style resolution
    std::vector<CSSRule> resolveStyles(const std::string& selector);
    std::vector<CSSRule> resolveStylesForElement(std::shared_ptr<ElementNode> element);
    
    // CSS generation
    std::string generateOptimizedCSS();
    std::string generateMinifiedCSS();
    std::string generateFormattedCSS();
    
    // Style analysis
    std::vector<std::string> getUnusedSelectors();
    std::vector<std::string> getConflictingSelectors();
    std::map<std::string, int> getSelectorUsage();
    
private:
    CHTLStyleProcessor processor;
    std::vector<CSSRule> all_rules;
    std::map<std::string, std::vector<CSSRule>> rules_by_selector;
    std::map<std::string, std::shared_ptr<TemplateNode>> style_templates;
    std::map<std::string, std::shared_ptr<CustomNode>> style_customs;
    
    // Style analysis
    void analyzeStyleUsage();
    void detectConflicts();
    void optimizeRules();
};

} // namespace CHTL

#endif // CHTLSTYLE_H