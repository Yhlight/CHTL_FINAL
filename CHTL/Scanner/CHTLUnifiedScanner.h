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
    const std::map<std::string, std::string>& getPlaceholders() const;

private:
    const std::string& source;
    size_t current = 0;
    int line = 1;
    std::map<std::string, std::string> placeholder_map;
    int placeholder_counter = 0;

    // Main scanning logic
    void process_script_block(std::vector<CodeFragment>& fragments, size_t& pos);
    void process_style_block(std::vector<CodeFragment>& fragments, size_t& pos);
    void process_chtl_block(std::vector<CodeFragment>& fragments, size_t& pos);

    // Helper methods
    std::string create_placeholder(const std::string& content);
    size_t find_matching_brace(size_t start_pos);
    size_t find_matching_brace_in_string(const std::string& str, size_t start_pos);
    void skip_whitespace_and_comments(size_t& pos);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
