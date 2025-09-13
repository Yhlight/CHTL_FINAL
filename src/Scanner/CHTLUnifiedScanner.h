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
    const std::map<std::string, std::string>& getPlaceholderMap() const { return placeholder_map_; };

private:
    std::string process_script_block(const std::string& content);
    std::string process_style_block(const std::string& content);

    const std::string& source_;
    std::vector<CodeChunk> chunks_;
    std::map<std::string, std::string> placeholder_map_;
    size_t current_ = 0;
    int placeholder_id_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
