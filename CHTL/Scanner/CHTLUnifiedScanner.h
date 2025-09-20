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
    const std::map<std::string, std::string>& getPlaceholderMap() const;

private:
    const std::string& source;
    std::map<std::string, std::string> placeholder_map;
    int placeholder_counter = 0;
    int current = 0;
    int line = 1;

    // Core scanning logic
    std::string processScript(const std::string& content);
    std::string processStyle(const std::string& content);

    // Placeholder management
    std::string addPlaceholder(const std::string& content, bool is_style);

    // Helper methods for scanning
    bool isAtEnd();
    char advance();
    char peek();
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
