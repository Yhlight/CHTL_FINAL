#pragma once

#include <string>
#include <vector>
#include <utility>

namespace chtl {

enum class CodeSliceType {
    CHTL,
    CHTL_JS,
    JS,
    CSS,
};

struct CodeSlice {
    CodeSliceType type;
    std::string content;
};

class UnifiedScanner {
public:
    // Scan full source into typed slices with placeholder encoding applied where needed.
    static std::vector<CodeSlice> scan(const std::string& source);

    // Encode JS/CSS placeholders to fully separate from CHTL/CHTL JS.
    static std::string encodePlaceholders(const std::string& source);

    // Decode placeholders back to original JS/CSS.
    static std::string decodePlaceholders(const std::string& sourceWithPlaceholders);
};

} // namespace chtl

