#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// Add CHTL JS compiler components
#include "../CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJS/CHTLJSParser/CHTLJSContext.h"


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

    // Scans the source for script blocks, compiles the CHTL JS parts,
    // and returns the final standard JS with placeholders for the original pure JS parts.
    ScanningResult scan();

private:
    // Recursively processes the content of a script block to isolate CHTL JS syntax.
    std::string isolateChtlJs(const std::string& script_content);

    // Helper methods for script processing.
    std::string generatePlaceholder();
    size_t findEndOfChtlJsBlock(const std::string& content, size_t block_start_pos);

    const std::string& source_;
    std::map<std::string, std::string> placeholder_map_;
    int placeholder_id_ = 0;
    const std::vector<std::string> chtljs_keywords_;

    // Context to share between CHTL JS parser instances
    std::shared_ptr<CHTLJS::CHTLJSContext> chtljs_context_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
