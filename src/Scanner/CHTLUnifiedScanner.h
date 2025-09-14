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

    const std::map<std::string, std::string>& getPlaceholderMap() const {
        return placeholder_map_;
    }

private:
    void process();
    void handleScriptTag(size_t brace_open_pos);
    void handleStyleTag(size_t brace_open_pos);

    // Helper for script handling
    std::string processScriptContent(const std::string& script_content);
    std::string generatePlaceholder();
    size_t findEndOfChtlJsBlock(const std::string& content, size_t block_start_pos);

    const std::string& source_;
    std::vector<CodeChunk> chunks_;
    std::map<std::string, std::string> placeholder_map_;
    size_t current_ = 0;
    int placeholder_id_ = 0;
    const std::vector<std::string> chtljs_keywords_;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
