#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace CHTL {

struct ScanningResult {
    std::string modified_source;
    std::map<std::string, std::string> placeholder_map;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    ScanningResult scan();

private:
    // Main processing logic
    void process();

    // Helper methods for scanning and state management
    void advance(size_t n = 1);
    char peek(size_t offset = 0) const;
    bool isAtEnd() const;
    void skipIrrelevant();
    std::string identifyKeywordAt(size_t pos);
    size_t findNextKeyword();

    // Methods for finding construct boundaries
    size_t findEndOfConstruct(const std::string& keyword);

    void flushJsChunk(size_t end);

    // Member variables
    const std::string& source_;
    std::stringstream processed_content_;
    std::map<std::string, std::string> placeholder_map_;

    size_t cursor_ = 0;
    size_t last_flush_pos_ = 0;
    int placeholder_id_ = 0;

    const std::vector<std::string> keywords_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
