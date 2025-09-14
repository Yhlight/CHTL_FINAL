#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Defines the type of code fragment identified by the scanner.
enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS
};

// Represents a fragment of code from the source file.
struct CodeFragment {
    FragmentType type;
    std::string content;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    // Scans the source file and partitions it into code fragments.
    std::vector<CodeFragment> scan();

private:
    const std::string& source_;
    size_t cursor_ = 0;

    // Helper to find the end of a balanced-brace block.
    size_t findEndOfBlock(size_t block_start_pos);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
