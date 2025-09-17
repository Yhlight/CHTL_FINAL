// This is the new CHTLUnifiedScanner header file.
// The implementation will follow the new design principles.
#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// The result of scanning a script block.
struct ScannerResult {
    std::string modified_content;
    std::map<std::string, std::string> placeholder_map;
};

// The Unified Scanner's role is to take a raw string of script content
// and separate the pure JS from the CHTL-JS constructs using placeholders.
class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& script_content);
    ScannerResult scan();

private:
    std::string script_content_;
    std::map<std::string, std::string> placeholder_map_;
    int placeholder_id_ = 0;

    std::string createPlaceholder(const std::string& content);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
