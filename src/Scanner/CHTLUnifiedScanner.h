#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <regex>

namespace CHTL {

enum class ChunkType {
    CHTL,
    JAVASCRIPT,
    CSS,
    CHTL_JS
};

struct CodeChunk {
    ChunkType type;
    std::string content;
    // Placeholders can be stored in a separate map in the scanner
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    std::vector<CodeChunk> scan();
    const std::map<std::string, std::string>& getPlaceholders() const;


private:
    size_t findMatchingBrace(size_t start_pos);
    void findAndProcessTags();
    void processScriptBlock(const std::string& block_content);
    void processStyleBlock(const std::string& block_content);
    void processChtlContent(size_t from, size_t to);

    std::string source_;
    std::vector<CodeChunk> chunks_;
    std::map<std::string, std::string> placeholder_map_;
    size_t placeholder_id_ = 0;

    // Regex to find script and style tags
    static const std::regex TAG_REGEX;
    // Regex to find CHTL JS specific syntax
    static const std::regex CHTL_JS_REGEX;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
