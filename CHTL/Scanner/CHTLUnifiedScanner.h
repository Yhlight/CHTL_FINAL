#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Defines the state of the scanner, indicating which language context it's currently in.
enum class ScannerState {
    SCANNING_CHTL,
    SCANNING_CSS,
    SCANNING_SCRIPT
};


class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();

private:
    // Source code and position tracking
    const std::string& source;
    size_t current = 0;
    int line = 1;

    // State machine
    ScannerState state = ScannerState::SCANNING_CHTL;
    std::vector<CodeFragment> fragments;

    // Helper methods for scanning
    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    void skipWhitespace();

    // Main scanning loop and state-specific handlers
    void scanToken();
    void scanChtl();
    void scanScript();
    void scanStyle();

    // Utility methods
    bool match(const std::string& expected);
    void addFragment(FragmentType type, const std::string& content);

    // CHTL JS keyword detection
    const std::map<std::string, bool> chtlJsKeywords = {
        {"Listen", true}, {"Animate", true}, {"Router", true},
        {"Delegate", true}, {"ScriptLoader", true}, {"Vir", true},
        {"iNeverAway", true}, {"printMylove", true}, {"util", true}
    };
    bool isChtlJsKeyword(const std::string& word);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
