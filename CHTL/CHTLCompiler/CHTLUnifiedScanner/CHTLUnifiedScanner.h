#ifndef CHTLUNIFIEDSCANNER_H
#define CHTLUNIFIEDSCANNER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

// Scanner modes
enum class ScannerMode {
    Broad,      // Broad judgment mode
    Strict,     // Strict judgment mode
    Adaptive    // Adaptive mode
};

// Slice types
enum class SliceType {
    Fixed,      // Fixed length slice
    Variable,   // Variable length slice
    Dynamic     // Dynamic length slice
};

// Placeholder types
enum class PlaceholderType {
    Dollar,         // $
    DollarQuestion, // $?
    DollarExclamation, // $!
    DollarUnderscore,  // $_
    Ellipsis,       // ...
    Custom          // Custom placeholder
};

// Slice configuration
struct SliceConfig {
    SliceType type;
    int min_length;
    int max_length;
    bool allow_overlap;
    bool preserve_whitespace;
    std::string delimiter;
    
    SliceConfig() : type(SliceType::Fixed), min_length(1), max_length(100), 
                   allow_overlap(false), preserve_whitespace(true), delimiter("") {}
};

// Placeholder configuration
struct PlaceholderConfig {
    PlaceholderType type;
    std::string pattern;
    std::string replacement;
    bool required;
    bool case_sensitive;
    
    PlaceholderConfig() : type(PlaceholderType::Dollar), pattern("$"), 
                         replacement(""), required(false), case_sensitive(true) {}
};

// Scan result
struct ScanResult {
    std::string content;
    std::vector<std::string> slices;
    std::map<std::string, std::string> placeholders;
    std::vector<std::string> tokens;
    std::map<std::string, std::string> metadata;
    bool success;
    std::string error_message;
    
    ScanResult() : success(false) {}
};

// Variable length slicer
class CHTLVariableLengthSlicer {
public:
    CHTLVariableLengthSlicer();
    ~CHTLVariableLengthSlicer();
    
    // Slicing operations
    std::vector<std::string> slice(const std::string& content, const SliceConfig& config) const;
    std::vector<std::string> sliceByLength(const std::string& content, int length) const;
    std::vector<std::string> sliceByDelimiter(const std::string& content, const std::string& delimiter) const;
    std::vector<std::string> sliceByPattern(const std::string& content, const std::string& pattern) const;
    
    // Advanced slicing
    std::vector<std::string> adaptiveSlice(const std::string& content, const SliceConfig& config) const;
    std::vector<std::string> contextAwareSlice(const std::string& content, const SliceConfig& config) const;
    std::vector<std::string> semanticSlice(const std::string& content, const SliceConfig& config) const;
    
    // Slice validation
    bool validateSlice(const std::string& slice, const SliceConfig& config) const;
    std::vector<std::string> validateSlices(const std::vector<std::string>& slices, const SliceConfig& config) const;
    
    // Slice utilities
    std::string mergeSlices(const std::vector<std::string>& slices) const;
    std::vector<std::string> optimizeSlices(const std::vector<std::string>& slices) const;
    std::map<std::string, int> analyzeSliceDistribution(const std::vector<std::string>& slices) const;
    
private:
    // Helper methods
    std::vector<std::string> splitByLength(const std::string& content, int length) const;
    std::vector<std::string> splitByDelimiter(const std::string& content, const std::string& delimiter) const;
    std::vector<std::string> splitByRegex(const std::string& content, const std::string& pattern) const;
    bool isValidSliceLength(const std::string& slice, int min_len, int max_len) const;
    std::string cleanSlice(const std::string& slice, bool preserve_whitespace) const;
};

// Intelligent expander
class CHTLIntelligentExpander {
public:
    CHTLIntelligentExpander();
    ~CHTLIntelligentExpander();
    
    // Expansion operations
    std::string expand(const std::string& content, const std::map<std::string, std::string>& context) const;
    std::string expandPlaceholders(const std::string& content, const std::map<std::string, std::string>& placeholders) const;
    std::string expandVariables(const std::string& content, const std::map<std::string, std::string>& variables) const;
    std::string expandTemplates(const std::string& content, const std::map<std::string, std::string>& templates) const;
    
    // Smart expansion
    std::string smartExpand(const std::string& content, const std::map<std::string, std::string>& context) const;
    std::string contextAwareExpand(const std::string& content, const std::map<std::string, std::string>& context) const;
    std::string semanticExpand(const std::string& content, const std::map<std::string, std::string>& context) const;
    
    // Expansion validation
    bool validateExpansion(const std::string& content, const std::string& expanded) const;
    std::vector<std::string> getExpansionErrors(const std::string& content, const std::string& expanded) const;
    
    // Expansion utilities
    std::map<std::string, std::string> extractPlaceholders(const std::string& content) const;
    std::vector<std::string> extractVariables(const std::string& content) const;
    std::vector<std::string> extractTemplates(const std::string& content) const;
    
private:
    // Helper methods
    std::string replacePlaceholder(const std::string& content, const std::string& placeholder, const std::string& value) const;
    std::string replaceVariable(const std::string& content, const std::string& variable, const std::string& value) const;
    std::string replaceTemplate(const std::string& content, const std::string& template_name, const std::string& value) const;
    bool isPlaceholder(const std::string& str) const;
    bool isVariable(const std::string& str) const;
    bool isTemplate(const std::string& str) const;
    std::string sanitizeExpansion(const std::string& content) const;
};

// Placeholder mechanism
class CHTLPlaceholderMechanism {
public:
    CHTLPlaceholderMechanism();
    ~CHTLPlaceholderMechanism();
    
    // Placeholder operations
    std::string createPlaceholder(const PlaceholderType& type, const std::string& value = "") const;
    std::string createPlaceholder(const PlaceholderConfig& config) const;
    std::string replacePlaceholder(const std::string& content, const std::string& placeholder, const std::string& value) const;
    std::string replacePlaceholders(const std::string& content, const std::map<std::string, std::string>& placeholders) const;
    
    // Placeholder detection
    std::vector<std::string> findPlaceholders(const std::string& content) const;
    std::vector<std::string> findPlaceholders(const std::string& content, const PlaceholderType& type) const;
    bool hasPlaceholder(const std::string& content, const std::string& placeholder) const;
    bool hasPlaceholder(const std::string& content, const PlaceholderType& type) const;
    
    // Placeholder validation
    bool validatePlaceholder(const std::string& placeholder) const;
    bool validatePlaceholders(const std::vector<std::string>& placeholders) const;
    std::vector<std::string> getPlaceholderErrors(const std::string& placeholder) const;
    
    // Placeholder utilities
    std::string generatePlaceholderId(const PlaceholderType& type) const;
    std::string generatePlaceholderId(const std::string& base) const;
    PlaceholderType detectPlaceholderType(const std::string& placeholder) const;
    std::string normalizePlaceholder(const std::string& placeholder) const;
    
private:
    // Helper methods
    std::string placeholderTypeToString(const PlaceholderType& type) const;
    PlaceholderType stringToPlaceholderType(const std::string& str) const;
    bool isValidPlaceholderChar(char c) const;
    std::string escapePlaceholder(const std::string& placeholder) const;
    std::string unescapePlaceholder(const std::string& placeholder) const;
};

// Broad/Strict judgment
class CHTLJudgmentSystem {
public:
    CHTLJudgmentSystem();
    ~CHTLJudgmentSystem();
    
    // Judgment operations
    bool judge(const std::string& content, const ScannerMode& mode) const;
    bool broadJudge(const std::string& content) const;
    bool strictJudge(const std::string& content) const;
    bool adaptiveJudge(const std::string& content) const;
    
    // Judgment with context
    bool judgeWithContext(const std::string& content, const std::map<std::string, std::string>& context, const ScannerMode& mode) const;
    bool judgeWithRules(const std::string& content, const std::vector<std::string>& rules, const ScannerMode& mode) const;
    bool judgeWithPatterns(const std::string& content, const std::vector<std::string>& patterns, const ScannerMode& mode) const;
    
    // Judgment validation
    bool validateJudgment(const std::string& content, const ScannerMode& mode) const;
    std::vector<std::string> getJudgmentErrors(const std::string& content, const ScannerMode& mode) const;
    
    // Judgment utilities
    std::string generateJudgmentReport(const std::string& content, const ScannerMode& mode) const;
    std::map<std::string, bool> getJudgmentResults(const std::string& content, const std::vector<ScannerMode>& modes) const;
    ScannerMode recommendMode(const std::string& content) const;
    
private:
    // Helper methods
    bool matchesPattern(const std::string& content, const std::string& pattern) const;
    bool matchesRule(const std::string& content, const std::string& rule) const;
    bool isContentValid(const std::string& content) const;
    bool isContentComplete(const std::string& content) const;
    bool isContentConsistent(const std::string& content) const;
    std::string analyzeContent(const std::string& content) const;
};

// Unified scanner
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();
    
    // Main scanning operations
    ScanResult scan(const std::string& content, const ScannerMode& mode = ScannerMode::Adaptive) const;
    ScanResult scanWithConfig(const std::string& content, const SliceConfig& slice_config, const ScannerMode& mode) const;
    ScanResult scanWithPlaceholders(const std::string& content, const std::vector<PlaceholderConfig>& placeholder_configs, const ScannerMode& mode) const;
    
    // Advanced scanning
    ScanResult smartScan(const std::string& content, const std::map<std::string, std::string>& context) const;
    ScanResult contextAwareScan(const std::string& content, const std::map<std::string, std::string>& context) const;
    ScanResult semanticScan(const std::string& content, const std::map<std::string, std::string>& context) const;
    
    // Batch scanning
    std::vector<ScanResult> scanBatch(const std::vector<std::string>& contents, const ScannerMode& mode) const;
    std::vector<ScanResult> scanBatchWithConfig(const std::vector<std::string>& contents, const SliceConfig& slice_config, const ScannerMode& mode) const;
    
    // Scan validation
    bool validateScan(const ScanResult& result) const;
    std::vector<std::string> getScanErrors(const ScanResult& result) const;
    bool validateScanContent(const std::string& content) const;
    
    // Scan utilities
    std::string generateScanReport(const ScanResult& result) const;
    std::map<std::string, std::string> getScanStatistics(const ScanResult& result) const;
    std::string optimizeScanResult(const ScanResult& result) const;
    
    // Configuration management
    void setDefaultSliceConfig(const SliceConfig& config);
    void setDefaultPlaceholderConfigs(const std::vector<PlaceholderConfig>& configs);
    void setDefaultScannerMode(const ScannerMode& mode);
    
    SliceConfig getDefaultSliceConfig() const;
    std::vector<PlaceholderConfig> getDefaultPlaceholderConfigs() const;
    ScannerMode getDefaultScannerMode() const;
    
private:
    CHTLVariableLengthSlicer slicer;
    CHTLIntelligentExpander expander;
    CHTLPlaceholderMechanism placeholder_mechanism;
    CHTLJudgmentSystem judgment_system;
    
    SliceConfig default_slice_config;
    std::vector<PlaceholderConfig> default_placeholder_configs;
    ScannerMode default_mode;
    
    // Helper methods
    ScanResult performScan(const std::string& content, const SliceConfig& slice_config, const std::vector<PlaceholderConfig>& placeholder_configs, const ScannerMode& mode) const;
    std::vector<std::string> tokenizeContent(const std::string& content) const;
    std::map<std::string, std::string> extractMetadata(const std::string& content) const;
    bool validateScanResult(const ScanResult& result) const;
    std::string generateErrorReport(const std::string& error) const;
};

} // namespace CHTL

#endif // CHTLUNIFIEDSCANNER_H