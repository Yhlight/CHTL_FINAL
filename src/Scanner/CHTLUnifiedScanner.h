#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// The result of the scanner's preprocessing.
// It contains the modified source with placeholders and the map to decode them.
struct ScanningResult {
    std::string modified_source;
    std::map<std::string, std::string> placeholder_map;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    // Scans the source for script blocks and replaces their JS content with placeholders.
    ScanningResult scan();

private:
    // Recursively processes the content of a script block.
    std::string processScriptContent(const std::string& script_content);

    // Helper methods for script processing.
    std::string generatePlaceholder();
    size_t findEndOfChtlJsBlock(const std::string& content, size_t block_start_pos);

    const std::string& source_;
    std::map<std::string, std::string> placeholder_map_;
    int placeholder_id_ = 0;
    const std::vector<std::string> chtljs_keywords_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
