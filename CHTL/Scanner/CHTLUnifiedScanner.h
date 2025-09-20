#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>
#include <set>

namespace CHTL {

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();

private:
    const std::string& source;
    size_t current = 0;
    int line = 1;

    // --- State Management ---
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int line_start = 1;

    // --- Core Scanning Logic ---
    void scanNextFragment();
    void finalizeCurrentFragment(FragmentType type);

    // --- Block Handlers ---
    void handleStyleBlock();
    void handleScriptBlock();
    void scanJsAndChtlJs(const std::string& script_source);

    // --- Helper Methods ---
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(const std::string& expected);
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    void skipStringLiteral();
    bool isChtlJsKeyword();

    // --- CHTL JS Keyword Recognition ---
    // Using a set for efficient lookup of CHTL JS keywords.
    const std::set<std::string> CHTL_JS_KEYWORDS = {
        "Listen", "Delegate", "Animate", "Vir", "Router", "ScriptLoader", "util"
    };
    const std::string JS_PLACEHOLDER = "_JS_CODE_PLACEHOLDER_";
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
