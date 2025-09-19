#pragma once

#include <string>
#include <vector>
#include <variant>

namespace CHTL {

enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS,
};

struct ExtractedBlock {
    std::string content;
    FragmentType type;
    std::string placeholder;
};

struct ScanResult {
    std::string modified_source;
    std::vector<ExtractedBlock> extracted_blocks;
};

class UnifiedScanner {
public:
    UnifiedScanner(const std::string& source);
    ScanResult scan();

private:
    std::string source;
};

} // namespace CHTL
