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
    // Source and position management
    const std::string& source;
    size_t current = 0;
    int line = 1;

    // State for script processing
    std::map<std::string, std::string> js_placeholders;
    int placeholder_id_counter = 0;

    // Main scanning loop
    void scan_toplevel(std::vector<CodeFragment>& fragments);

    // Block-specific processors
    void process_style_block(std::vector<CodeFragment>& fragments, int block_start_line);
    bool is_chtl_in_css_statement(const std::string& statement);
    void process_script_block(std::vector<CodeFragment>& fragments, int block_start_line);
    std::string scan_script_recursive(const std::string& input, std::map<std::string, std::string>& placeholders);

    // Utility methods
    bool is_at_end();
    char advance();
    char peek(size_t offset = 0);
    bool match(const std::string& expected);
    bool match_whole_word(const std::string& word);
    void skip_whitespace_and_comments();
    std::string consume_block_content();
    std::string get_next_placeholder_id();
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
