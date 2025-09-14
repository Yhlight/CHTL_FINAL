#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class ChunkType {
    CHTL,
    JavaScript,
    Css,
    ChtlJs,
    Placeholder
};

struct CodeChunk {
    ChunkType type;
    std::string content;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    std::vector<CodeChunk> scan();
    const std::map<std::string, std::string>& getPlaceholderMap() const;

private:
    void scanSource();
    void scanScriptBlock();
    void scanStyleBlock();
    void scanChtl();

    // Helpers for script scanning
    void scanJavaScriptContent(size_t block_start, size_t block_end);
    bool isChtlJsKeyword(size_t position);
    size_t findEndOfChtlJsBlock(size_t position);
    size_t findEnhancedSelector(size_t position);

    // Placeholder management
    std::string createPlaceholder(const std::string& content);
    void addChunk(ChunkType type, const std::string& content);

    // Utility helpers
    char peek();
    char advance();
    bool isAtEnd();
    bool match(const std::string& expected);
    void skipWhitespace();

    const std::string& source_;
    std::vector<CodeChunk> chunks_;
    std::map<std::string, std::string> placeholder_map_;
    size_t current_ = 0;
    size_t start_ = 0;
    int placeholder_id_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
