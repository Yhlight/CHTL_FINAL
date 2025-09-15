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
    UNKNOWN
};

// Represents a fragment of code identified by the scanner.
struct CodeFragment {
    std::string content;
    FragmentType type;
    std::string placeholder; // The key used to replace this fragment.
};


// The result of a full scan operation.
struct ScanResult {
    // The main CHTL source code with non-CHTL blocks replaced by placeholders.
    std::string chtl_with_placeholders;
    // A map from placeholder keys to the original code fragments.
    std::map<std::string, CodeFragment> placeholder_map;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    ScanResult scan();

private:
    // Represents the current parsing context.
    enum class ScannerContext {
        GENERAL
        // No longer need IN_SCRIPT or IN_STYLE as we treat those blocks as opaque
    };

    // Main processing logic
    void processGeneral();

    // Helper methods for scanning and state management
    void advance(size_t n = 1);
    char peek(size_t offset = 0) const;
    bool isAtEnd() const;
    std::string identifyKeywordAt(size_t pos);
    size_t findNextKeyword();

    // Methods for finding construct boundaries
    size_t findEndOfBlock(char open_brace, char close_brace);

    // Placeholder and fragment management
    std::string createPlaceholder(FragmentType type);
    void flushChunk(size_t end);

    // Member variables
    const std::string& source_;
    ScanResult result_; // The result object being built during the scan.

    size_t cursor_ = 0;
    size_t last_flush_pos_ = 0;
    int placeholder_id_ = 0;
    ScannerContext context_ = ScannerContext::GENERAL;

    const std::map<ScannerContext, std::vector<std::string>> context_keywords_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
