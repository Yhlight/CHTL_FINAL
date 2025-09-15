#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace CHTL {

// Represents the different language fragments the scanner can identify.
enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS,
    // RESPONSIVE_VALUE removed as it's handled by the CHTL parser
    UNKNOWN
};

// The output of the scanner will be a vector of these fragments.
struct CodeFragment {
    std::string content;
    FragmentType type;
    // The placeholder key if this fragment was replaced.
    std::string placeholder;
};

// The final result of the scanning process.
struct ScanningResult {
    // The main CHTL source code with all non-CHTL parts replaced by placeholders.
    std::string chtl_with_placeholders;
    // A map from placeholder to the original code fragment.
    std::map<std::string, CodeFragment> placeholder_map;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    ScanningResult scan();

private:
    // Represents the current parsing context.
    enum class ScannerContext {
        GENERAL,
        IN_SCRIPT,
        IN_STYLE
    };

    // Main processing logic
    void process();
    void processGeneral();
    void processScript(size_t script_end_pos);
    void processStyle();

    // Helper methods for scanning and state management
    void advance(size_t n = 1);
    char peek(size_t offset = 0) const;
    bool isAtEnd() const;
    void skipWhitespace();
    std::string identifyKeywordAt(size_t pos);
    size_t findNextKeyword();

    // Methods for finding construct boundaries
    size_t findEndOfBlock(char open_brace, char close_brace);
    size_t findEndOfConstruct(const std::string& keyword);

    // Placeholder and fragment management
    std::string createPlaceholder(const std::string& content, FragmentType type);
    void flushChunk(size_t end, FragmentType type);

    // Member variables
    const std::string& source_;
    std::vector<CodeFragment> fragments_;

    size_t cursor_ = 0;
    size_t last_flush_pos_ = 0;
    int placeholder_id_ = 0;
    ScannerContext context_ = ScannerContext::GENERAL;

    const std::map<ScannerContext, std::vector<std::string>> context_keywords_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
