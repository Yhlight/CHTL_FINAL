#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace CHTL {

// Represents a block of pure CSS code that should be passed directly to the CSS compiler.
struct CssFragment {
    std::string content;
};

// The result of the unified scanning process.
struct ScannedData {
    std::string chtl_source; // The main CHTL source, with JS blocks replaced by placeholders.
    std::map<std::string, std::string> js_placeholder_map; // Maps placeholders to the original JS code.
    std::vector<CssFragment> css_fragments; // Any pure CSS global style blocks.
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    // The main entry point for scanning.
    ScannedData scan();

private:
    // Main processing loop that builds the ScannedData.
    void process();

    // Scans a script {} block and returns its content with JS replaced by placeholders.
    std::string processScriptBlock(size_t block_start_pos);

    // Scans a global <style> tag and adds its content to the css_fragments.
    void processGlobalStyleBlock(size_t block_start_pos);

    // Helper to identify CHTL JS keywords within a script block.
    bool isChtlJsKeyword(const std::string& script_content, size_t pos);

    // Helper methods for scanning and state management
    char peek(size_t offset = 0) const;
    bool isAtEnd(size_t pos) const;
    size_t findEndOfBlock(size_t start_pos, char open_brace, char close_brace);
    size_t findEndOfTag(size_t start_pos, const std::string& tag);
    std::string createJsPlaceholder(const std::string& content);

    // Member variables
    const std::string& source_;
    ScannedData scanned_data_;
    int placeholder_id_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
