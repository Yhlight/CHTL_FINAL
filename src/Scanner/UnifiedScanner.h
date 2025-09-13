#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <unordered_map>

namespace CHTL {

struct ScanResult {
    std::string modified_source;
    std::unordered_map<std::string, std::string> placeholders;
};

class UnifiedScanner {
public:
    explicit UnifiedScanner(const std::string& source);

    ScanResult scan();

private:
    void scanForBlocks(std::string& source, const std::string& block_name, const std::string& placeholder_prefix, bool sub_scan = false);
    void scanForSubstrings(std::string& source, const std::string& start_delim, const std::string& end_delim, const std::string& placeholder_prefix);
    std::string swissCheeseScanScript(const std::string& script_content);

    const std::string& source_;
    size_t placeholder_counter_ = 0;
    std::unordered_map<std::string, std::string> placeholders_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
