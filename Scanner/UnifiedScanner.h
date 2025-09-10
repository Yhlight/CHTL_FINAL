#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class LanguageType {
    CHTL,
    CHTL_JS,
    CSS,
    JAVASCRIPT
};

struct CodeFragment {
    LanguageType type;
    std::string content;
};

class UnifiedScanner {
public:
    std::vector<CodeFragment> Scan(const std::string& source);
};

} // namespace CHTL
