#include "CHTLStyle.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>

namespace CHTL {

CHTLStyleProcessor::CHTLStyleProcessor() 
    : auto_generate_classes(true), auto_generate_ids(true), enable_context_inference(true),
      class_prefix("chtl-"), id_prefix("chtl-") {
}

std::vector<CSSRule> CHTLStyleProcessor::processStyleBlock(std::shared_ptr<StyleNode> style_node, const CSSContext& context) {
    std::vector<CSSRule> rules;
    
    if (!style_node) return rules;
    
    // Process each style rule in the block
    for (const auto& rule_node : style_node->style_rules) {
        if (!rule_node) continue;
        
        // Parse the rule text
        std::string rule_text = rule_node->toHTML();
        CSSRule rule = parseCSSRule(rule_text, context);
        
        if (!rule.selector.empty()) {
            rules.push_back(rule);
        }
    }
    
    return rules;
}

std::string CHTLStyleProcessor::generateCSS(const std::vector<CSSRule>& rules) {
    std::ostringstream css;
    
    for (const auto& rule : rules) {
        css << rule.selector << " {\n";
        
        for (const auto& property : rule.properties) {
            css << "  " << property.first << ": " << property.second << ";\n";
        }
        
        css << "}\n\n";
    }
    
    return css.str();
}

CSSContext CHTLStyleProcessor::deriveContext(std::shared_ptr<ElementNode> element, const CSSContext& parent_context) {
    CSSContext context;
    
    if (element) {
        context.element_name = element->tag_name;
        context.parent_selector = parent_context.parent_selector;
        
        // Extract class and id attributes
        for (const auto& attr : element->attributes) {
            if (attr->attribute_name == "class") {
                context.class_names = split(attr->attribute_value, ' ');
            } else if (attr->attribute_name == "id") {
                context.id_names = {attr->attribute_value};
            } else {
                context.attributes[attr->attribute_name] = attr->attribute_value;
            }
        }
        
        // Generate auto class/ID if needed
        if (auto_generate_classes && context.class_names.empty()) {
            std::string auto_class = generateAutoClass(element->tag_name, context.attributes);
            if (!auto_class.empty()) {
                context.class_names.push_back(auto_class);
            }
        }
        
        if (auto_generate_ids && context.id_names.empty()) {
            std::string auto_id = generateAutoId(element->tag_name, context.attributes);
            if (!auto_id.empty()) {
                context.id_names.push_back(auto_id);
            }
        }
        
        // Build current selector
        std::string current_selector = generateSelector(context);
        if (!parent_context.parent_selector.empty()) {
            context.parent_selector = parent_context.parent_selector + " " + current_selector;
        } else {
            context.parent_selector = current_selector;
        }
    }
    
    return context;
}

std::string CHTLStyleProcessor::generateSelector(const CSSContext& context, const std::string& base_selector) {
    std::ostringstream selector;
    
    if (!base_selector.empty()) {
        selector << base_selector;
    } else {
        // Generate selector based on context
        if (!context.element_name.empty()) {
            selector << context.element_name;
        }
        
        // Add ID
        if (!context.id_names.empty()) {
            selector << "#" << context.id_names[0];
        }
        
        // Add classes
        for (const auto& class_name : context.class_names) {
            selector << "." << class_name;
        }
    }
    
    return selector.str();
}

std::string CHTLStyleProcessor::generateAutoClass(const std::string& element_name, const std::map<std::string, std::string>& /* attributes */) {
    std::ostringstream class_name;
    class_name << class_prefix << element_name;
    
    // Add counter for uniqueness
    int counter = class_counters[element_name]++;
    if (counter > 0) {
        class_name << "-" << counter;
    }
    
    return class_name.str();
}

std::string CHTLStyleProcessor::generateAutoId(const std::string& element_name, const std::map<std::string, std::string>& /* attributes */) {
    std::ostringstream id_name;
    id_name << id_prefix << element_name;
    
    // Add counter for uniqueness
    int counter = id_counters[element_name]++;
    if (counter > 0) {
        id_name << "-" << counter;
    }
    
    return id_name.str();
}

std::string CHTLStyleProcessor::evaluatePropertyCondition(const std::string& expression, const CSSContext& context) {
    // Simple property condition evaluation
    // This can be enhanced to support complex expressions
    
    if (expression.empty()) return "";
    
    // Check for attribute conditions
    if (expression[0] == '[' && expression.back() == ']') {
        std::string attr_expr = expression.substr(1, expression.length() - 2);
        return processPointingAttribute(attr_expr, context);
    }
    
    // Check for context references
    if (expression[0] == '&') {
        return resolveContextReference(expression, context);
    }
    
    return expression;
}

bool CHTLStyleProcessor::evaluateLogicalExpression(const std::string& expression, const CSSContext& context) {
    // Simple logical expression evaluation
    // This can be enhanced to support complex boolean logic
    
    if (expression.empty()) return true;
    
    // Split by logical operators
    std::vector<std::string> tokens = split(expression, ' ');
    
    bool result = true;
    std::string current_op = "&&";
    
    for (const auto& token : tokens) {
        if (isLogicalOperator(token)) {
            current_op = token;
        } else {
            bool token_result = evaluatePropertyCondition(token, context) != "";
            
            if (current_op == "&&") {
                result = result && token_result;
            } else if (current_op == "||") {
                result = result || token_result;
            }
        }
    }
    
    return result;
}

CSSRule CHTLStyleProcessor::parseCSSRule(const std::string& rule_text, const CSSContext& context) {
    CSSRule rule("", CSSRuleType::Element);
    
    // Simple CSS rule parsing
    // This can be enhanced to support complex CSS syntax
    
    size_t brace_pos = rule_text.find('{');
    if (brace_pos == std::string::npos) return rule;
    
    std::string selector_text = trim(rule_text.substr(0, brace_pos));
    std::string properties_text = rule_text.substr(brace_pos + 1);
    
    // Remove closing brace
    size_t end_brace = properties_text.find('}');
    if (end_brace != std::string::npos) {
        properties_text = properties_text.substr(0, end_brace);
    }
    
    // Parse selector
    rule.selector = inferContextSelector(selector_text, context);
    rule.rule_type = CSSRuleType::Element; // Default type
    
    // Parse properties
    rule.properties = parseCSSProperties(properties_text);
    
    // Calculate specificity
    rule.specificity = calculateSpecificity(rule.selector);
    
    return rule;
}

std::map<std::string, std::string> CHTLStyleProcessor::parseCSSProperties(const std::string& properties_text) {
    std::map<std::string, std::string> properties;
    
    std::istringstream stream(properties_text);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line.back() != ';') continue;
        
        // Remove semicolon
        line = line.substr(0, line.length() - 1);
        
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;
        
        std::string property = trim(line.substr(0, colon_pos));
        std::string value = trim(line.substr(colon_pos + 1));
        
        if (isValidCSSProperty(property) && isValidCSSValue(value)) {
            properties[property] = value;
        }
    }
    
    return properties;
}

size_t CHTLStyleProcessor::calculateSpecificity(const std::string& selector) {
    size_t specificity = 0;
    
    // Count IDs
    size_t id_count = std::count(selector.begin(), selector.end(), '#');
    specificity += id_count * 100;
    
    // Count classes and attributes
    size_t class_count = std::count(selector.begin(), selector.end(), '.');
    size_t attr_count = std::count(selector.begin(), selector.end(), '[');
    specificity += (class_count + attr_count) * 10;
    
    // Count elements
    size_t element_count = 0;
    for (char c : selector) {
        if (std::isalpha(c) && (element_count == 0 || !std::isalnum(selector[element_count - 1]))) {
            element_count++;
        }
    }
    specificity += element_count;
    
    return specificity;
}

size_t CHTLStyleProcessor::calculateSelectorSpecificity(const std::string& selector) {
    return calculateSpecificity(selector);
}

std::vector<CSSRule> CHTLStyleProcessor::optimizeCSS(const std::vector<CSSRule>& rules) {
    std::vector<CSSRule> optimized = rules;
    
    // Sort by specificity
    std::sort(optimized.begin(), optimized.end(), 
        [](const CSSRule& a, const CSSRule& b) {
            return a.specificity < b.specificity;
        });
    
    // Merge duplicate rules
    optimized = mergeDuplicateRules(optimized);
    
    return optimized;
}

std::vector<CSSRule> CHTLStyleProcessor::mergeDuplicateRules(const std::vector<CSSRule>& rules) {
    std::map<std::string, CSSRule> rule_map;
    
    for (const auto& rule : rules) {
        auto it = rule_map.find(rule.selector);
        if (it != rule_map.end()) {
            // Merge properties
            for (const auto& property : rule.properties) {
                it->second.properties[property.first] = property.second;
            }
        } else {
            rule_map[rule.selector] = rule;
        }
    }
    
    std::vector<CSSRule> merged;
    for (const auto& pair : rule_map) {
        merged.push_back(pair.second);
    }
    
    return merged;
}

std::string CHTLStyleProcessor::inferContextSelector(const std::string& selector, const CSSContext& context) {
    if (selector.empty()) return "";
    
    // Handle context reference (&)
    if (selector[0] == '&') {
        return resolveContextReference(selector, context);
    }
    
    // Handle class selectors
    if (selector[0] == '.') {
        std::string class_name = selector.substr(1);
        if (std::find(context.class_names.begin(), context.class_names.end(), class_name) != context.class_names.end()) {
            return context.parent_selector + selector;
        }
    }
    
    // Handle ID selectors
    if (selector[0] == '#') {
        std::string id_name = selector.substr(1);
        if (std::find(context.id_names.begin(), context.id_names.end(), id_name) != context.id_names.end()) {
            return context.parent_selector + selector;
        }
    }
    
    return selector;
}

std::string CHTLStyleProcessor::resolveContextReference(const std::string& reference, const CSSContext& context) {
    if (reference.empty()) return "";
    
    if (reference == "&") {
        return context.parent_selector;
    }
    
    if (reference.substr(0, 2) == "&:") {
        std::string pseudo = reference.substr(1);
        return context.parent_selector + pseudo;
    }
    
    return reference;
}

std::string CHTLStyleProcessor::processChainedExpression(const std::string& expression, const CSSContext& context) {
    // Process chained expressions like "a && b && c"
    std::vector<std::string> parts = split(expression, '&');
    std::ostringstream result;
    
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result << " && ";
        result << evaluatePropertyCondition(trim(parts[i]), context);
    }
    
    return result.str();
}

std::string CHTLStyleProcessor::processOptionalExpression(const std::string& expression, const CSSContext& context) {
    // Process optional expressions like "a ? b : c"
    size_t question_pos = expression.find('?');
    if (question_pos == std::string::npos) {
        return evaluatePropertyCondition(expression, context);
    }
    
    std::string condition = trim(expression.substr(0, question_pos));
    std::string remaining = expression.substr(question_pos + 1);
    
    size_t colon_pos = remaining.find(':');
    if (colon_pos == std::string::npos) {
        return evaluatePropertyCondition(condition, context);
    }
    
    std::string true_value = trim(remaining.substr(0, colon_pos));
    std::string false_value = trim(remaining.substr(colon_pos + 1));
    
    if (evaluateLogicalExpression(condition, context)) {
        return evaluatePropertyCondition(true_value, context);
    } else {
        return evaluatePropertyCondition(false_value, context);
    }
}

std::string CHTLStyleProcessor::processLogicalExpression(const std::string& expression, const CSSContext& context) {
    // Process logical expressions like "a || b && c"
    return processChainedExpression(expression, context);
}

std::string CHTLStyleProcessor::processPointingAttribute(const std::string& expression, const CSSContext& context) {
    // Process pointing attribute expressions like "[attr=value]"
    if (expression.empty()) return "";
    
    size_t equal_pos = expression.find('=');
    if (equal_pos == std::string::npos) {
        // Just attribute name
        auto it = context.attributes.find(expression);
        return (it != context.attributes.end()) ? it->second : "";
    }
    
    std::string attr_name = trim(expression.substr(0, equal_pos));
    std::string attr_value = trim(expression.substr(equal_pos + 1));
    
    // Remove quotes if present
    if (attr_value.length() >= 2 && 
        ((attr_value[0] == '"' && attr_value.back() == '"') ||
         (attr_value[0] == '\'' && attr_value.back() == '\''))) {
        attr_value = attr_value.substr(1, attr_value.length() - 2);
    }
    
    auto it = context.attributes.find(attr_name);
    if (it != context.attributes.end() && it->second == attr_value) {
        return attr_value;
    }
    
    return "";
}

std::string CHTLStyleProcessor::resolveAttributeReference(const std::string& reference, const CSSContext& context) {
    return processPointingAttribute(reference, context);
}

std::string CHTLStyleProcessor::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> CHTLStyleProcessor::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;
    
    while (std::getline(stream, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

bool CHTLStyleProcessor::isOperator(const std::string& token) {
    return token == "&&" || token == "||" || token == "?" || token == ":" || 
           token == "=" || token == "!=" || token == "<" || token == ">" ||
           token == "<=" || token == ">=";
}

bool CHTLStyleProcessor::isLogicalOperator(const std::string& token) {
    return token == "&&" || token == "||";
}

std::string CHTLStyleProcessor::getOperatorPrecedence(const std::string& op) {
    if (op == "&&") return "2";
    if (op == "||") return "1";
    if (op == "?" || op == ":") return "3";
    return "0";
}

bool CHTLStyleProcessor::isValidCSSProperty(const std::string& property) {
    // Simple CSS property validation
    // This can be enhanced with a comprehensive list
    return !property.empty() && std::isalpha(property[0]);
}

bool CHTLStyleProcessor::isValidCSSValue(const std::string& value) {
    // Simple CSS value validation
    return !value.empty();
}

CSSPropertyType CHTLStyleProcessor::getPropertyType(const std::string& property) {
    // Simple property type classification
    if (property.find("color") != std::string::npos) return CSSPropertyType::Color;
    if (property.find("background") != std::string::npos) return CSSPropertyType::Background;
    if (property.find("border") != std::string::npos) return CSSPropertyType::Border;
    if (property.find("width") != std::string::npos || property.find("height") != std::string::npos) return CSSPropertyType::Layout;
    if (property.find("font") != std::string::npos || property.find("text") != std::string::npos) return CSSPropertyType::Typography;
    if (property.find("margin") != std::string::npos || property.find("padding") != std::string::npos) return CSSPropertyType::Spacing;
    if (property.find("display") != std::string::npos) return CSSPropertyType::Display;
    if (property.find("position") != std::string::npos) return CSSPropertyType::Position;
    if (property.find("animation") != std::string::npos || property.find("transition") != std::string::npos) return CSSPropertyType::Animation;
    
    return CSSPropertyType::Other;
}

void CHTLStyleProcessor::pushContext(const CSSContext& context) {
    context_stack.push_back(context);
}

void CHTLStyleProcessor::popContext() {
    if (!context_stack.empty()) {
        context_stack.pop_back();
    }
}

CSSContext CHTLStyleProcessor::getCurrentContext() {
    if (context_stack.empty()) {
        return CSSContext();
    }
    return context_stack.back();
}

// CHTLStyleManager implementation
CHTLStyleManager::CHTLStyleManager() {
}

void CHTLStyleManager::processDocument(std::shared_ptr<RootNode> ast) {
    if (!ast) return;
    
    // Process all style blocks in the document
    processStyleBlocks(ast);
    
    // Analyze and optimize styles
    analyzeStyleUsage();
    optimizeRules();
}

void CHTLStyleManager::processStyleBlocks(std::shared_ptr<CHTLNode> node) {
    if (!node) return;
    
    // Process current node if it's a style block
    if (node->node_type == NodeType::Style) {
        auto style_node = std::static_pointer_cast<StyleNode>(node);
        CSSContext context;
        std::vector<CSSRule> rules = processor.processStyleBlock(style_node, context);
        
        for (const auto& rule : rules) {
            all_rules.push_back(rule);
            rules_by_selector[rule.selector].push_back(rule);
        }
    }
    
    // Process children
    for (const auto& child : node->children) {
        processStyleBlocks(child);
    }
}

std::string CHTLStyleManager::generateDocumentCSS() {
    return processor.generateCSS(all_rules);
}

void CHTLStyleManager::registerStyleBlock(std::shared_ptr<StyleNode> style_node, const CSSContext& context) {
    if (!style_node) return;
    
    std::vector<CSSRule> rules = processor.processStyleBlock(style_node, context);
    
    for (const auto& rule : rules) {
        all_rules.push_back(rule);
        rules_by_selector[rule.selector].push_back(rule);
    }
}

void CHTLStyleManager::registerTemplate(std::shared_ptr<TemplateNode> template_node) {
    if (template_node && template_node->template_type == "Style") {
        style_templates[template_node->template_name] = template_node;
    }
}

void CHTLStyleManager::registerCustom(std::shared_ptr<CustomNode> custom_node) {
    if (custom_node && custom_node->custom_type == "Style") {
        style_customs[custom_node->custom_name] = custom_node;
    }
}

std::vector<CSSRule> CHTLStyleManager::resolveStyles(const std::string& selector) {
    auto it = rules_by_selector.find(selector);
    if (it != rules_by_selector.end()) {
        return it->second;
    }
    return {};
}

std::vector<CSSRule> CHTLStyleManager::resolveStylesForElement(std::shared_ptr<ElementNode> element) {
    if (!element) return {};
    
    CSSContext context = processor.deriveContext(element);
    std::string selector = processor.generateSelector(context);
    
    return resolveStyles(selector);
}

std::string CHTLStyleManager::generateOptimizedCSS() {
    std::vector<CSSRule> optimized = processor.optimizeCSS(all_rules);
    return processor.generateCSS(optimized);
}

std::string CHTLStyleManager::generateMinifiedCSS() {
    std::string css = generateOptimizedCSS();
    
    // Simple minification - remove extra whitespace
    std::regex whitespace_regex("\\s+");
    css = std::regex_replace(css, whitespace_regex, " ");
    
    // Remove unnecessary spaces around certain characters
    css = std::regex_replace(css, std::regex("\\s*{\\s*"), "{");
    css = std::regex_replace(css, std::regex("\\s*}\\s*"), "}");
    css = std::regex_replace(css, std::regex("\\s*;\\s*"), ";");
    css = std::regex_replace(css, std::regex("\\s*:\\s*"), ":");
    
    return css;
}

std::string CHTLStyleManager::generateFormattedCSS() {
    return generateOptimizedCSS();
}

std::vector<std::string> CHTLStyleManager::getUnusedSelectors() {
    // This would require DOM analysis to determine which selectors are actually used
    // For now, return empty vector
    return {};
}

std::vector<std::string> CHTLStyleManager::getConflictingSelectors() {
    // This would require specificity analysis to detect conflicts
    // For now, return empty vector
    return {};
}

std::map<std::string, int> CHTLStyleManager::getSelectorUsage() {
    // This would require usage tracking
    // For now, return empty map
    return {};
}

void CHTLStyleManager::analyzeStyleUsage() {
    // Analyze style usage patterns
    // This can be enhanced with actual usage tracking
}

void CHTLStyleManager::optimizeRules() {
    all_rules = processor.optimizeCSS(all_rules);
}

} // namespace CHTL