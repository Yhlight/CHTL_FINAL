#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

struct ScanResult {
    std::string sanitized_source;
    std::map<std::string, std::string> placeholders;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    ScanResult scan();

private:
    enum class State {
        DEFAULT,
        IN_STYLE_BLOCK,
        IN_SCRIPT_BLOCK
    };

    void advance();
    char peek();
    bool isAtEnd();
    void skipWhitespace();

    void scanDefault();
    void scanStyleBlock();
    void scanScriptBlock();

    std::string extractBlockContent();

    const std::string& source;
    std::string sanitized_source_buffer;
    std::map<std::string, std::string> placeholders;

    size_t current_pos = 0;
    int style_placeholder_count = 0;
    int script_placeholder_count = 0;
    State current_state = State::DEFAULT;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
