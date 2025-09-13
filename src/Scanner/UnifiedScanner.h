#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>

namespace CHTL {

class UnifiedScanner {
public:
    explicit UnifiedScanner(const std::string& source);
    FragmentList scan();

private:
    void findNextFragment();
    void processChtlFragment(size_t end);
    void processScriptFragment();

    const std::string& source_;
    size_t current_ = 0;
    FragmentList fragments_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
