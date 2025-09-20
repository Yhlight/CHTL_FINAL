#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();

private:
    const std::string& source;
    int current = 0;
    int line = 1;

    // Helper methods for scanning
    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    void skipWhitespace();

    // Block processors
    void process_style_block(std::vector<CodeFragment>& fragments, int content_start, int content_end);
    void process_script_block(std::vector<CodeFragment>& fragments, int content_start, int content_end);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
