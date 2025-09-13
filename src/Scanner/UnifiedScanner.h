#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Struct to hold the separated code fragments
struct CodeFragments {
    std::string chtl;
    std::string chtl_js;
    std::string css;
    std::string js;
    // We might need to store mappings from placeholders back to original code
    std::map<std::string, std::string> js_placeholders;
};

class UnifiedScanner {
public:
    explicit UnifiedScanner(const std::string& source);
    CodeFragments scan();

private:
    void skipWhitespace();
    size_t findBlockEnd(size_t start_offset);
    void processScriptBlock(const std::string& content, CodeFragments& fragments);
    std::string createPlaceholder();

    const std::string& source_;
    size_t current_ = 0;
    size_t line_ = 1;
    int placeholder_counter_ = 0;
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H
