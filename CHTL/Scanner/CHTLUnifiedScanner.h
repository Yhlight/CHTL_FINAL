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
    std::string_view source;
    size_t current = 0;
    int line = 1;
    std::vector<CodeFragment> fragments;

    void processScriptBlock(std::string_view content, int start_line, std::vector<CodeFragment>& fragments);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
