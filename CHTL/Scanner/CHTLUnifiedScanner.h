#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();

private:
    void scanScriptContent(const std::string& script_content, std::vector<CodeFragment>& fragments);
    const std::string& source;
    int current = 0;
    int line = 1;

    // Helper methods for scanning
    bool isAtEnd();
    char advance();
    char peek();
    void skipWhitespaceAndComments();

    // The core logic to find and extract top-level blocks
    void findNextBlock(std::vector<CodeFragment>& fragments);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
