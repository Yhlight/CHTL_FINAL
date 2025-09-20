#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Defines the context in which the scanner is currently operating.
enum class ScannerState {
    TOP_LEVEL,
    IN_STYLE_BLOCK,
    IN_SCRIPT_BLOCK,
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();

private:
    // Source and position management
    const std::string& source;
    size_t current = 0;
    size_t start = 0;
    int line = 1;
    ScannerState state = ScannerState::TOP_LEVEL;

    // A stack to handle nested states, e.g., nested blocks.
    std::vector<ScannerState> state_stack;

    // Main scanning loop and state dispatching
    void scan_token(std::vector<CodeFragment>& fragments);

    // Context-specific scanning functions
    void scan_top_level(std::vector<CodeFragment>& fragments);
    void scan_style_block(std::vector<CodeFragment>& fragments);
    void scan_script_block(std::vector<CodeFragment>& fragments);

    // Placeholder mechanism for JS code within CHTL JS
    std::map<std::string, std::string> js_placeholders;
    int placeholder_id_counter = 0;
    std::string process_script_content(const std::string& content);

    // Utility methods
    bool is_at_end();
    char advance();
    char peek(size_t offset = 0);
    char peek_next();
    bool match(char expected);
    void skip_whitespace();
    void skip_line_comment();
    void skip_block_comment();
    void add_fragment(FragmentType type, std::vector<CodeFragment>& fragments, const std::string& content = "");
    bool is_alpha(char c);
    bool is_alpha_numeric(char c);
    std::string consume_identifier();

    std::string get_next_placeholder_id();
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
