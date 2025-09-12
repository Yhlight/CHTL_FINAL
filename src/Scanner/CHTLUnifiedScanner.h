#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Defines the types of code fragments the scanner can identify.
enum class FragmentType {
    CHTL,
    CHTL_JS,
    CSS,
    JS
};

// Holds the results of a scan operation.
struct ScannedFragments {
    std::map<FragmentType, std::string> fragments;
    std::map<std::string, std::string> placeholder_map;
};

class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner() = default;

    // Scans the source code and separates it into language-specific fragments.
    ScannedFragments scan(const std::string& source_code);

private:
    // Helper function to generate unique placeholders.
    std::string getNextPlaceholder(FragmentType type);

    size_t placeholder_counts_[4] = {0}; // CHTL, CHTL_JS, CSS, JS
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
