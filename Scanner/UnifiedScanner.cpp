#include "UnifiedScanner.h"

namespace CHTL {

std::vector<CodeFragment> UnifiedScanner::Scan(const std::string& source) {
    // Reverting to V1 of the scanner.
    // The full implementation of the Unified Scanner with its placeholder
    // mechanism is a major task that requires its own dedicated development cycle.
    // This version treats the entire source as a single CHTL fragment,
    // allowing the CHTL compiler to function for the features implemented so far.

    std::vector<CodeFragment> fragments;
    if (!source.empty()) {
        fragments.push_back({LanguageType::CHTL, source});
    }
    return fragments;
}

} // namespace CHTL
