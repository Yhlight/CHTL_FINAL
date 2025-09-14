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
    // Add an operator== for easy testing
    bool operator==(const CodeChunk& other) const {
        return type == other.type && content == other.content;
    }
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    std::vector<CodeChunk> scan();

private:
    void process();
    void handleScriptTag();
    void handleStyleTag();

    const std::string& source_;
    std::vector<CodeChunk> chunks_;
    size_t current_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
