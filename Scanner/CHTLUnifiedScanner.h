#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "../Util/Fragment.h"
#include <string>
#include <vector>

class CHTLUnifiedScanner {
public:
    /**
     * @brief Scans the source code and partitions it into fragments.
     * @param source The CHTL source code as a string.
     * @return A vector of CodeFragment objects.
     */
    std::vector<CodeFragment> scan(const std::string& source);
};

#endif // CHTL_UNIFIED_SCANNER_H
