#ifndef CHTL_UNIFIEDSCANNER_H
#define CHTL_UNIFIEDSCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class CodeBlockType {
    CHTL,
    GLOBAL_CSS,     // For global style blocks
    SCRIPT,         // For script blocks (JS + CHTL JS mix)
    // Other types can be added later
};

struct CodeBlock {
    CodeBlockType type;
    std::string content;
    std::string placeholder;
};

class UnifiedScanner {
public:
    explicit UnifiedScanner(std::string source);

    // Performs the scanning operation.
    void scan();

    // Returns the main CHTL source code with placeholders.
    std::string getProcessedSource() const;

    // Returns the list of extracted code blocks.
    const std::vector<CodeBlock>& getCodeBlocks() const;

private:
    std::string source_code_;
    std::string processed_source_;
    std::vector<CodeBlock> code_blocks_;
    size_t placeholder_id_ = 0;

    std::string generatePlaceholder(CodeBlockType type);
};

} // namespace CHTL

#endif // CHTL_UNIFIEDSCANNER_H
