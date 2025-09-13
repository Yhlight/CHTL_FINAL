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
    ChtlJs
};

struct CodeChunk {
    ChunkType type;
    std::string content;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);

    std::vector<CodeChunk> scan();

private:
    // Main scanning loop calls this
    void scanNextToken();

    // Handlers for different block types
    void handleScriptBlock();
    void handleStyleBlock();
    void handleBracketBlock(); // Handles [Origin], [Import], etc.
    void handleGenericChtl();  // Handles regular CHTL until a special block

    // CHTL JS specific parsers
    void consumeChtlJsBlock();
    void consumeChtlJsEnhancedSelector();
    void consumeChtlJsResponsiveValue();

    // Utility methods for scanning
    bool isAtEnd() const;
    char advance();
    char peek() const;
    bool match(const std::string& expected);
    void skipWhitespace();
    void addChunk(ChunkType type);

    const std::string& source_;
    std::vector<CodeChunk> chunks_;
    size_t current_pos_ = 0;
    size_t start_pos_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
