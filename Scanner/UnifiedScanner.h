#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Defines the type of code in a fragment.
enum class CodeType {
    UNKNOWN,
    CHTL,
    CSS,
    JS,
    CHTL_JS
};

// Represents a snippet of code identified by the scanner.
struct CodeFragment {
    CodeType type;
    std::string content;
};

// The main class for scanning and separating source code.
class UnifiedScanner {
public:
    UnifiedScanner(const std::string& source);

    // The primary method to perform the scan.
    // It will populate the internal list of fragments.
    void scan();

    // Get the resulting code fragments after a scan.
    const std::vector<CodeFragment>& getFragments() const;

private:
    std::string m_source;
    size_t m_cursor = 0;
    std::vector<CodeFragment> m_fragments;

    // The core of the scanner will be a state machine.
    // These are helper functions for the state machine.
    void scanRoot(); // The initial state
    // ... other state-specific scan functions will be added here ...

    // Helper to advance the cursor and check for EOF
    bool isEOF() const;
    char peek() const;
    void advance();
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H
