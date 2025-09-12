#include "CHTLUnifiedScanner.h"
#include <sstream>

namespace CHTL {

// Scans the source code and separates it into language-specific fragments.
ScannedFragments CHTLUnifiedScanner::scan(const std::string& source_code) {
    // For now, this is a stub implementation.
    // The core logic for scanning and separating fragments will be built here.
    // It will iterate through the source_code, identify blocks of CHTL, JS, CSS, etc.,
    // and use the placeholder mechanism to separate them.

    ScannedFragments result;
    // By default, assume the entire source is a single CHTL fragment.
    result.fragments[FragmentType::CHTL] = source_code;

    return result;
}

// Helper function to generate unique placeholders.
std::string CHTLUnifiedScanner::getNextPlaceholder(FragmentType type) {
    std::stringstream placeholder;
    switch (type) {
        case FragmentType::CHTL:
            placeholder << "__CHTL_PLACEHOLDER_CHTL_" << placeholder_counts_[0]++;
            break;
        case FragmentType::CHTL_JS:
            placeholder << "__CHTL_PLACEHOLDER_CHTL_JS_" << placeholder_counts_[1]++;
            break;
        case FragmentType::CSS:
            placeholder << "__CHTL_PLACEHOLDER_CSS_" << placeholder_counts_[2]++;
            break;
        case FragmentType::JS:
            placeholder << "__CHTL_PLACEHOLDER_JS_" << placeholder_counts_[3]++;
            break;
    }
    return placeholder.str();
}

} // namespace CHTL
