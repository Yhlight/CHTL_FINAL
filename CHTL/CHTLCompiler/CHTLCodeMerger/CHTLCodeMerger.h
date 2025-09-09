#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Code fragment types
enum class FragmentType {
    HTML,
    CSS,
    JavaScript,
    CHTL,
    CHTLJS,
    Mixed
};

// Code fragment
struct CodeFragment {
    FragmentType type;
    std::string content;
    std::string source_file;
    int priority;
    
    CodeFragment() : type(FragmentType::HTML), priority(0) {}
    CodeFragment(FragmentType t, const std::string& c) : type(t), content(c), priority(0) {}
    CodeFragment(FragmentType t, const std::string& c, const std::string& f) : type(t), content(c), source_file(f), priority(0) {}
};

// Merge strategy
enum class MergeStrategy {
    Sequential,     // Merge in order
    Priority,       // Merge by priority
    Type,          // Merge by type
    Dependency,    // Merge by dependency
    Custom         // Custom merge function
};

// Merge configuration
struct MergeConfig {
    MergeStrategy strategy;
    bool minify;
    bool compress;
    bool remove_comments;
    bool remove_whitespace;
    
    MergeConfig() : strategy(MergeStrategy::Sequential), minify(false), 
                   compress(false), remove_comments(false), remove_whitespace(false) {}
};

// Merge result
struct MergeResult {
    bool success;
    std::string merged_content;
    std::string error_message;
    std::vector<std::string> warnings;
    
    MergeResult() : success(false) {}
};

// Simple Code Merger
class CHTLCodeMerger {
private:
    bool debug_mode;
    bool verbose;
    
public:
    CHTLCodeMerger();
    ~CHTLCodeMerger();
    
    // Merging
    MergeResult merge(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    MergeResult merge(const std::vector<CodeFragment>& fragments, MergeStrategy strategy = MergeStrategy::Sequential);
    MergeResult merge(const std::vector<std::string>& files, FragmentType type, const MergeConfig& config);
    
    // Configuration
    void setDebugMode(bool enabled);
    void setVerboseMode(bool enabled);
    
    // Utility
    FragmentType detectFragmentType(const std::string& content) const;
    
    // Fragment management
    CodeFragment createFragment(const std::string& content, FragmentType type, const std::string& source_file = "");
    std::vector<CodeFragment> createFragmentsFromFiles(const std::vector<std::string>& files);
    
    // Validation
    bool validateFragment(const CodeFragment& fragment) const;
    bool validateFragments(const std::vector<CodeFragment>& fragments) const;
    
private:
    std::string mergeHTMLFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    std::string mergeCSSFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    std::string mergeJavaScriptFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    std::string mergeCHTLFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    std::string mergeCHTLJSFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    std::string mergeMixedFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    
    std::string minifyCSS(const std::string& css);
    std::string minifyJavaScript(const std::string& js);
    std::string removeComments(const std::string& content);
    std::string removeWhitespace(const std::string& content);
    
    std::string readFile(const std::string& file_path) const;
    bool writeFile(const std::string& file_path, const std::string& content) const;
    std::string getFileExtension(const std::string& file_path) const;
    bool fileExists(const std::string& file_path) const;
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H