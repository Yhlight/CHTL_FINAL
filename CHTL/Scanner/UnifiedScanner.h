#pragma once

#include <string>
#include <vector>
#include <map>

enum class CodeType {
    CHTL,
    CSS,
    JS,
    CHTL_JS,
    UNKNOWN
};

struct CodeSnippet {
    CodeType type;
    std::string content;
    // We can add more metadata later, like original line numbers
};

class UnifiedScanner {
public:
    UnifiedScanner() = default;
    std::vector<CodeSnippet> scan(const std::string& source);

private:
    // Helper methods to find and process different blocks
    void findBlocks(const std::string& source, const std::string& blockName, CodeType chtlType, CodeType nativeType);

    std::vector<CodeSnippet> snippets;
};
