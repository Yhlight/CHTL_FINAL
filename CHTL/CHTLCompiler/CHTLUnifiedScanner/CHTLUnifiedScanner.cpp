#include "CHTLUnifiedScanner.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <set>

namespace CHTL {

// CHTLVariableLengthSlicer implementation
CHTLVariableLengthSlicer::CHTLVariableLengthSlicer() {
}

CHTLVariableLengthSlicer::~CHTLVariableLengthSlicer() {
}

std::vector<std::string> CHTLVariableLengthSlicer::slice(const std::string& content, const SliceConfig& config) const {
    switch (config.type) {
        case SliceType::Fixed:
            return sliceByLength(content, config.min_length);
        case SliceType::Variable:
            return adaptiveSlice(content, config);
        case SliceType::Dynamic:
            return semanticSlice(content, config);
        default:
            return sliceByLength(content, config.min_length);
    }
}

std::vector<std::string> CHTLVariableLengthSlicer::sliceByLength(const std::string& content, int length) const {
    return splitByLength(content, length);
}

std::vector<std::string> CHTLVariableLengthSlicer::sliceByDelimiter(const std::string& content, const std::string& delimiter) const {
    return splitByDelimiter(content, delimiter);
}

std::vector<std::string> CHTLVariableLengthSlicer::sliceByPattern(const std::string& content, const std::string& pattern) const {
    return splitByRegex(content, pattern);
}

std::vector<std::string> CHTLVariableLengthSlicer::adaptiveSlice(const std::string& content, const SliceConfig& config) const {
    std::vector<std::string> slices;
    
    // Adaptive slicing based on content characteristics
    if (content.length() <= config.max_length) {
        slices.push_back(content);
        return slices;
    }
    
    // Try to slice by natural boundaries first
    std::vector<std::string> delimiter_slices = splitByDelimiter(content, " ");
    if (delimiter_slices.size() > 1) {
        std::vector<std::string> result;
        std::string current_slice;
        
        for (const auto& slice : delimiter_slices) {
            if (current_slice.length() + slice.length() + 1 <= config.max_length) {
                if (!current_slice.empty()) {
                    current_slice += " ";
                }
                current_slice += slice;
            } else {
                if (!current_slice.empty()) {
                    result.push_back(current_slice);
                }
                current_slice = slice;
            }
        }
        
        if (!current_slice.empty()) {
            result.push_back(current_slice);
        }
        
        return result;
    }
    
    // Fall back to fixed length slicing
    return splitByLength(content, config.min_length);
}

std::vector<std::string> CHTLVariableLengthSlicer::contextAwareSlice(const std::string& content, const SliceConfig& config) const {
    // Context-aware slicing considers surrounding content
    return adaptiveSlice(content, config);
}

std::vector<std::string> CHTLVariableLengthSlicer::semanticSlice(const std::string& content, const SliceConfig& config) const {
    // Semantic slicing tries to preserve meaning
    return adaptiveSlice(content, config);
}

bool CHTLVariableLengthSlicer::validateSlice(const std::string& slice, const SliceConfig& config) const {
    return isValidSliceLength(slice, config.min_length, config.max_length);
}

std::vector<std::string> CHTLVariableLengthSlicer::validateSlices(const std::vector<std::string>& slices, const SliceConfig& config) const {
    std::vector<std::string> valid_slices;
    
    for (const auto& slice : slices) {
        if (validateSlice(slice, config)) {
            valid_slices.push_back(slice);
        }
    }
    
    return valid_slices;
}

std::string CHTLVariableLengthSlicer::mergeSlices(const std::vector<std::string>& slices) const {
    std::string result;
    
    for (size_t i = 0; i < slices.size(); ++i) {
        if (i > 0) {
            result += " ";
        }
        result += slices[i];
    }
    
    return result;
}

std::vector<std::string> CHTLVariableLengthSlicer::optimizeSlices(const std::vector<std::string>& slices) const {
    std::vector<std::string> optimized;
    
    for (const auto& slice : slices) {
        std::string cleaned = cleanSlice(slice, true);
        if (!cleaned.empty()) {
            optimized.push_back(cleaned);
        }
    }
    
    return optimized;
}

std::map<std::string, int> CHTLVariableLengthSlicer::analyzeSliceDistribution(const std::vector<std::string>& slices) const {
    std::map<std::string, int> distribution;
    
    for (const auto& slice : slices) {
        distribution[slice] = slice.length();
    }
    
    return distribution;
}

std::vector<std::string> CHTLVariableLengthSlicer::splitByLength(const std::string& content, int length) const {
    std::vector<std::string> slices;
    
    for (size_t i = 0; i < content.length(); i += length) {
        slices.push_back(content.substr(i, length));
    }
    
    return slices;
}

std::vector<std::string> CHTLVariableLengthSlicer::splitByDelimiter(const std::string& content, const std::string& delimiter) const {
    std::vector<std::string> slices;
    std::stringstream ss(content);
    std::string slice;
    
    while (std::getline(ss, slice, delimiter[0])) {
        if (!slice.empty()) {
            slices.push_back(slice);
        }
    }
    
    return slices;
}

std::vector<std::string> CHTLVariableLengthSlicer::splitByRegex(const std::string& content, const std::string& pattern) const {
    std::vector<std::string> slices;
    std::regex regex_pattern(pattern);
    std::sregex_token_iterator iter(content.begin(), content.end(), regex_pattern, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        slices.push_back(*iter);
    }
    
    return slices;
}

bool CHTLVariableLengthSlicer::isValidSliceLength(const std::string& slice, int min_len, int max_len) const {
    return slice.length() >= min_len && slice.length() <= max_len;
}

std::string CHTLVariableLengthSlicer::cleanSlice(const std::string& slice, bool preserve_whitespace) const {
    if (preserve_whitespace) {
        return slice;
    }
    
    std::string result = slice;
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

// CHTLIntelligentExpander implementation
CHTLIntelligentExpander::CHTLIntelligentExpander() {
}

CHTLIntelligentExpander::~CHTLIntelligentExpander() {
}

std::string CHTLIntelligentExpander::expand(const std::string& content, const std::map<std::string, std::string>& context) const {
    std::string result = content;
    
    // Expand placeholders first
    result = expandPlaceholders(result, context);
    
    // Expand variables
    result = expandVariables(result, context);
    
    // Expand templates
    result = expandTemplates(result, context);
    
    return result;
}

std::string CHTLIntelligentExpander::expandPlaceholders(const std::string& content, const std::map<std::string, std::string>& placeholders) const {
    std::string result = content;
    
    for (const auto& pair : placeholders) {
        result = replacePlaceholder(result, pair.first, pair.second);
    }
    
    return result;
}

std::string CHTLIntelligentExpander::expandVariables(const std::string& content, const std::map<std::string, std::string>& variables) const {
    std::string result = content;
    
    for (const auto& pair : variables) {
        result = replaceVariable(result, pair.first, pair.second);
    }
    
    return result;
}

std::string CHTLIntelligentExpander::expandTemplates(const std::string& content, const std::map<std::string, std::string>& templates) const {
    std::string result = content;
    
    for (const auto& pair : templates) {
        result = replaceTemplate(result, pair.first, pair.second);
    }
    
    return result;
}

std::string CHTLIntelligentExpander::smartExpand(const std::string& content, const std::map<std::string, std::string>& context) const {
    // Smart expansion with context awareness
    return contextAwareExpand(content, context);
}

std::string CHTLIntelligentExpander::contextAwareExpand(const std::string& content, const std::map<std::string, std::string>& context) const {
    // Context-aware expansion
    return expand(content, context);
}

std::string CHTLIntelligentExpander::semanticExpand(const std::string& content, const std::map<std::string, std::string>& context) const {
    // Semantic expansion preserving meaning
    return expand(content, context);
}

bool CHTLIntelligentExpander::validateExpansion(const std::string& content, const std::string& expanded) const {
    // Simple validation - check if expansion is valid
    return !expanded.empty() && expanded.length() >= content.length();
}

std::vector<std::string> CHTLIntelligentExpander::getExpansionErrors(const std::string& content, const std::string& expanded) const {
    std::vector<std::string> errors;
    
    if (expanded.empty()) {
        errors.push_back("Expansion result is empty");
    }
    
    if (expanded.length() < content.length()) {
        errors.push_back("Expansion result is shorter than original content");
    }
    
    return errors;
}

std::map<std::string, std::string> CHTLIntelligentExpander::extractPlaceholders(const std::string& content) const {
    std::map<std::string, std::string> placeholders;
    
    // Simple placeholder extraction
    std::regex placeholder_regex(R"(\$\{(\w+)\})");
    std::sregex_iterator iter(content.begin(), content.end(), placeholder_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        placeholders[match[1].str()] = "";
    }
    
    return placeholders;
}

std::vector<std::string> CHTLIntelligentExpander::extractVariables(const std::string& content) const {
    std::vector<std::string> variables;
    
    // Simple variable extraction
    std::regex variable_regex(R"(\$\{(\w+)\})");
    std::sregex_iterator iter(content.begin(), content.end(), variable_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        variables.push_back(match[1].str());
    }
    
    return variables;
}

std::vector<std::string> CHTLIntelligentExpander::extractTemplates(const std::string& content) const {
    std::vector<std::string> templates;
    
    // Simple template extraction
    std::regex template_regex(R"(\{\{(\w+)\}\})");
    std::sregex_iterator iter(content.begin(), content.end(), template_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        templates.push_back(match[1].str());
    }
    
    return templates;
}

std::string CHTLIntelligentExpander::replacePlaceholder(const std::string& content, const std::string& placeholder, const std::string& value) const {
    std::string result = content;
    std::string pattern = "${" + placeholder + "}";
    
    size_t pos = 0;
    while ((pos = result.find(pattern, pos)) != std::string::npos) {
        result.replace(pos, pattern.length(), value);
        pos += value.length();
    }
    
    return result;
}

std::string CHTLIntelligentExpander::replaceVariable(const std::string& content, const std::string& variable, const std::string& value) const {
    return replacePlaceholder(content, variable, value);
}

std::string CHTLIntelligentExpander::replaceTemplate(const std::string& content, const std::string& template_name, const std::string& value) const {
    std::string result = content;
    std::string pattern = "{{" + template_name + "}}";
    
    size_t pos = 0;
    while ((pos = result.find(pattern, pos)) != std::string::npos) {
        result.replace(pos, pattern.length(), value);
        pos += value.length();
    }
    
    return result;
}

bool CHTLIntelligentExpander::isPlaceholder(const std::string& str) const {
    return str.length() >= 3 && str[0] == '$' && str[1] == '{' && str[str.length() - 1] == '}';
}

bool CHTLIntelligentExpander::isVariable(const std::string& str) const {
    return isPlaceholder(str);
}

bool CHTLIntelligentExpander::isTemplate(const std::string& str) const {
    return str.length() >= 4 && str[0] == '{' && str[1] == '{' && str[str.length() - 2] == '}' && str[str.length() - 1] == '}';
}

std::string CHTLIntelligentExpander::sanitizeExpansion(const std::string& content) const {
    // Simple sanitization
    std::string result = content;
    
    // Remove any remaining placeholders
    std::regex placeholder_regex(R"(\$\{\w+\})");
    result = std::regex_replace(result, placeholder_regex, "");
    
    // Remove any remaining templates
    std::regex template_regex(R"(\{\{\w+\}\})");
    result = std::regex_replace(result, template_regex, "");
    
    return result;
}

// CHTLPlaceholderMechanism implementation
CHTLPlaceholderMechanism::CHTLPlaceholderMechanism() {
}

CHTLPlaceholderMechanism::~CHTLPlaceholderMechanism() {
}

std::string CHTLPlaceholderMechanism::createPlaceholder(const PlaceholderType& type, const std::string& value) const {
    std::string placeholder = placeholderTypeToString(type);
    
    if (!value.empty()) {
        placeholder += "{" + value + "}";
    }
    
    return placeholder;
}

std::string CHTLPlaceholderMechanism::createPlaceholder(const PlaceholderConfig& config) const {
    std::string placeholder = config.pattern;
    
    if (!config.replacement.empty()) {
        placeholder += "{" + config.replacement + "}";
    }
    
    return placeholder;
}

std::string CHTLPlaceholderMechanism::replacePlaceholder(const std::string& content, const std::string& placeholder, const std::string& value) const {
    std::string result = content;
    
    size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
        result.replace(pos, placeholder.length(), value);
        pos += value.length();
    }
    
    return result;
}

std::string CHTLPlaceholderMechanism::replacePlaceholders(const std::string& content, const std::map<std::string, std::string>& placeholders) const {
    std::string result = content;
    
    for (const auto& pair : placeholders) {
        result = replacePlaceholder(result, pair.first, pair.second);
    }
    
    return result;
}

std::vector<std::string> CHTLPlaceholderMechanism::findPlaceholders(const std::string& content) const {
    std::vector<std::string> placeholders;
    
    // Find all placeholder patterns
    std::regex placeholder_regex(R"(\$\{?\w+\}?)");
    std::sregex_iterator iter(content.begin(), content.end(), placeholder_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        placeholders.push_back(iter->str());
    }
    
    return placeholders;
}

std::vector<std::string> CHTLPlaceholderMechanism::findPlaceholders(const std::string& content, const PlaceholderType& type) const {
    std::vector<std::string> placeholders;
    std::string pattern = placeholderTypeToString(type);
    
    // Find placeholders of specific type
    std::regex placeholder_regex(pattern + R"(\{?\w+\}?)");
    std::sregex_iterator iter(content.begin(), content.end(), placeholder_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        placeholders.push_back(iter->str());
    }
    
    return placeholders;
}

bool CHTLPlaceholderMechanism::hasPlaceholder(const std::string& content, const std::string& placeholder) const {
    return content.find(placeholder) != std::string::npos;
}

bool CHTLPlaceholderMechanism::hasPlaceholder(const std::string& content, const PlaceholderType& type) const {
    std::string pattern = placeholderTypeToString(type);
    return content.find(pattern) != std::string::npos;
}

bool CHTLPlaceholderMechanism::validatePlaceholder(const std::string& placeholder) const {
    if (placeholder.empty()) {
        return false;
    }
    
    // Check if placeholder starts with valid character
    if (!isValidPlaceholderChar(placeholder[0])) {
        return false;
    }
    
    return true;
}

bool CHTLPlaceholderMechanism::validatePlaceholders(const std::vector<std::string>& placeholders) const {
    for (const auto& placeholder : placeholders) {
        if (!validatePlaceholder(placeholder)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> CHTLPlaceholderMechanism::getPlaceholderErrors(const std::string& placeholder) const {
    std::vector<std::string> errors;
    
    if (placeholder.empty()) {
        errors.push_back("Placeholder is empty");
    }
    
    if (!placeholder.empty() && !isValidPlaceholderChar(placeholder[0])) {
        errors.push_back("Placeholder starts with invalid character");
    }
    
    return errors;
}

std::string CHTLPlaceholderMechanism::generatePlaceholderId(const PlaceholderType& type) const {
    return placeholderTypeToString(type) + "_" + std::to_string(std::hash<std::string>{}(placeholderTypeToString(type)));
}

std::string CHTLPlaceholderMechanism::generatePlaceholderId(const std::string& base) const {
    return base + "_" + std::to_string(std::hash<std::string>{}(base));
}

PlaceholderType CHTLPlaceholderMechanism::detectPlaceholderType(const std::string& placeholder) const {
    if (placeholder.empty()) {
        return PlaceholderType::Dollar;
    }
    
    char first_char = placeholder[0];
    
    switch (first_char) {
        case '$':
            if (placeholder.length() > 1) {
                char second_char = placeholder[1];
                switch (second_char) {
                    case '?': return PlaceholderType::DollarQuestion;
                    case '!': return PlaceholderType::DollarExclamation;
                    case '_': return PlaceholderType::DollarUnderscore;
                    default: return PlaceholderType::Dollar;
                }
            }
            return PlaceholderType::Dollar;
        case '.':
            if (placeholder.length() >= 3 && placeholder.substr(0, 3) == "...") {
                return PlaceholderType::Ellipsis;
            }
            return PlaceholderType::Custom;
        default:
            return PlaceholderType::Custom;
    }
}

std::string CHTLPlaceholderMechanism::normalizePlaceholder(const std::string& placeholder) const {
    std::string result = placeholder;
    
    // Remove extra whitespace
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    
    // Ensure proper format
    if (result[0] == '$' && result.length() > 1 && result[1] != '{') {
        result = result.substr(0, 1) + "{" + result.substr(1) + "}";
    }
    
    return result;
}

std::string CHTLPlaceholderMechanism::placeholderTypeToString(const PlaceholderType& type) const {
    switch (type) {
        case PlaceholderType::Dollar: return "$";
        case PlaceholderType::DollarQuestion: return "$?";
        case PlaceholderType::DollarExclamation: return "$!";
        case PlaceholderType::DollarUnderscore: return "$_";
        case PlaceholderType::Ellipsis: return "...";
        case PlaceholderType::Custom: return "custom";
        default: return "$";
    }
}

PlaceholderType CHTLPlaceholderMechanism::stringToPlaceholderType(const std::string& str) const {
    if (str == "$") return PlaceholderType::Dollar;
    if (str == "$?") return PlaceholderType::DollarQuestion;
    if (str == "$!") return PlaceholderType::DollarExclamation;
    if (str == "$_") return PlaceholderType::DollarUnderscore;
    if (str == "...") return PlaceholderType::Ellipsis;
    return PlaceholderType::Custom;
}

bool CHTLPlaceholderMechanism::isValidPlaceholderChar(char c) const {
    return c == '$' || c == '.' || std::isalnum(c) || c == '_';
}

std::string CHTLPlaceholderMechanism::escapePlaceholder(const std::string& placeholder) const {
    std::string result = placeholder;
    
    // Escape special characters
    size_t pos = 0;
    while ((pos = result.find("$", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\$");
        pos += 2;
    }
    
    return result;
}

std::string CHTLPlaceholderMechanism::unescapePlaceholder(const std::string& placeholder) const {
    std::string result = placeholder;
    
    // Unescape special characters
    size_t pos = 0;
    while ((pos = result.find("\\$", pos)) != std::string::npos) {
        result.replace(pos, 2, "$");
        pos += 1;
    }
    
    return result;
}

// CHTLJudgmentSystem implementation
CHTLJudgmentSystem::CHTLJudgmentSystem() {
}

CHTLJudgmentSystem::~CHTLJudgmentSystem() {
}

bool CHTLJudgmentSystem::judge(const std::string& content, const ScannerMode& mode) const {
    switch (mode) {
        case ScannerMode::Broad:
            return broadJudge(content);
        case ScannerMode::Strict:
            return strictJudge(content);
        case ScannerMode::Adaptive:
            return adaptiveJudge(content);
        default:
            return adaptiveJudge(content);
    }
}

bool CHTLJudgmentSystem::broadJudge(const std::string& content) const {
    // Broad judgment - more lenient
    return !content.empty() && isContentValid(content);
}

bool CHTLJudgmentSystem::strictJudge(const std::string& content) const {
    // Strict judgment - more rigorous
    return !content.empty() && isContentValid(content) && isContentComplete(content) && isContentConsistent(content);
}

bool CHTLJudgmentSystem::adaptiveJudge(const std::string& content) const {
    // Adaptive judgment - adjusts based on content
    if (content.empty()) {
        return false;
    }
    
    // Start with broad judgment
    bool broad_result = broadJudge(content);
    
    // If content is short, use strict judgment
    if (content.length() < 100) {
        return strictJudge(content);
    }
    
    // For longer content, use broad judgment
    return broad_result;
}

bool CHTLJudgmentSystem::judgeWithContext(const std::string& content, const std::map<std::string, std::string>& context, const ScannerMode& mode) const {
    // Context-aware judgment
    bool base_result = judge(content, mode);
    
    if (!base_result) {
        return false;
    }
    
    // Additional context-based validation
    for (const auto& pair : context) {
        if (content.find(pair.first) != std::string::npos) {
            // Content contains context key, validate further
            if (pair.second.empty()) {
                return false;
            }
        }
    }
    
    return true;
}

bool CHTLJudgmentSystem::judgeWithRules(const std::string& content, const std::vector<std::string>& rules, const ScannerMode& mode) const {
    bool base_result = judge(content, mode);
    
    if (!base_result) {
        return false;
    }
    
    // Apply additional rules
    for (const auto& rule : rules) {
        if (!matchesRule(content, rule)) {
            return false;
        }
    }
    
    return true;
}

bool CHTLJudgmentSystem::judgeWithPatterns(const std::string& content, const std::vector<std::string>& patterns, const ScannerMode& mode) const {
    bool base_result = judge(content, mode);
    
    if (!base_result) {
        return false;
    }
    
    // Check patterns
    for (const auto& pattern : patterns) {
        if (!matchesPattern(content, pattern)) {
            return false;
        }
    }
    
    return true;
}

bool CHTLJudgmentSystem::validateJudgment(const std::string& content, const ScannerMode& mode) const {
    return judge(content, mode);
}

std::vector<std::string> CHTLJudgmentSystem::getJudgmentErrors(const std::string& content, const ScannerMode& mode) const {
    std::vector<std::string> errors;
    
    if (content.empty()) {
        errors.push_back("Content is empty");
    }
    
    if (!isContentValid(content)) {
        errors.push_back("Content is invalid");
    }
    
    if (mode == ScannerMode::Strict) {
        if (!isContentComplete(content)) {
            errors.push_back("Content is incomplete");
        }
        
        if (!isContentConsistent(content)) {
            errors.push_back("Content is inconsistent");
        }
    }
    
    return errors;
}

std::string CHTLJudgmentSystem::generateJudgmentReport(const std::string& content, const ScannerMode& mode) const {
    std::stringstream report;
    
    report << "Judgment Report\n";
    report << "==============\n";
    report << "Content length: " << content.length() << "\n";
    report << "Mode: " << (mode == ScannerMode::Broad ? "Broad" : mode == ScannerMode::Strict ? "Strict" : "Adaptive") << "\n";
    report << "Result: " << (judge(content, mode) ? "PASS" : "FAIL") << "\n";
    
    auto errors = getJudgmentErrors(content, mode);
    if (!errors.empty()) {
        report << "Errors:\n";
        for (const auto& error : errors) {
            report << "  - " << error << "\n";
        }
    }
    
    return report.str();
}

std::map<std::string, bool> CHTLJudgmentSystem::getJudgmentResults(const std::string& content, const std::vector<ScannerMode>& modes) const {
    std::map<std::string, bool> results;
    
    for (const auto& mode : modes) {
        std::string mode_name = (mode == ScannerMode::Broad ? "Broad" : mode == ScannerMode::Strict ? "Strict" : "Adaptive");
        results[mode_name] = judge(content, mode);
    }
    
    return results;
}

ScannerMode CHTLJudgmentSystem::recommendMode(const std::string& content) const {
    if (content.empty()) {
        return ScannerMode::Strict;
    }
    
    if (content.length() < 50) {
        return ScannerMode::Strict;
    } else if (content.length() < 500) {
        return ScannerMode::Adaptive;
    } else {
        return ScannerMode::Broad;
    }
}

bool CHTLJudgmentSystem::matchesPattern(const std::string& content, const std::string& pattern) const {
    try {
        std::regex regex_pattern(pattern);
        return std::regex_search(content, regex_pattern);
    } catch (...) {
        return false;
    }
}

bool CHTLJudgmentSystem::matchesRule(const std::string& content, const std::string& rule) const {
    // Simple rule matching
    return content.find(rule) != std::string::npos;
}

bool CHTLJudgmentSystem::isContentValid(const std::string& content) const {
    return !content.empty() && content.length() > 0;
}

bool CHTLJudgmentSystem::isContentComplete(const std::string& content) const {
    // Simple completeness check
    return content.length() > 10;
}

bool CHTLJudgmentSystem::isContentConsistent(const std::string& content) const {
    // Simple consistency check
    return content.length() > 0;
}

std::string CHTLJudgmentSystem::analyzeContent(const std::string& content) const {
    std::stringstream analysis;
    
    analysis << "Content Analysis\n";
    analysis << "Length: " << content.length() << "\n";
    analysis << "Characters: " << content.size() << "\n";
    analysis << "Words: " << std::count(content.begin(), content.end(), ' ') + 1 << "\n";
    
    return analysis.str();
}

} // namespace CHTL