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
    // Main scanning loop logic
    void process();

    // Block handlers
    bool handleScriptBlock(size_t keyword_pos, size_t& last_pos);
    bool handleStyleBlock(size_t keyword_pos, size_t& last_pos);
    bool handleOriginBlock(size_t keyword_pos, size_t& last_pos);
    void handleChtlBlock();

    // Helpers
    std::string scanScriptContent(const std::string& content);
    bool isWordBoundary(const std::string& source, size_t pos, size_t word_len);
    size_t findMatchingBrace(const std::string& source, size_t start_pos);

    const std::string& source_;
    std::vector<CodeChunk> chunks_;
    std::map<std::string, std::string> placeholder_map_;
    size_t current_ = 0;
    int placeholder_id_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
