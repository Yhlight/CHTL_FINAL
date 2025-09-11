#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <unordered_map>

struct ScannedSource {
    std::string chtlSource;
    std::unordered_map<std::string, std::string> scriptBlocks; // Placeholder -> Script Content
};

class UnifiedScanner {
public:
    ScannedSource scan(const std::string& source);
};

#endif // CHTL_UNIFIED_SCANNER_H
