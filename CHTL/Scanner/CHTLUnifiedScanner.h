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
    const std::string& source;
    size_t current = 0;
    int line = 1;

    // Helper methods for scanning
    bool isAtEnd();
    char advance();
    char peek();
    void skipWhitespaceAndComments();
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
