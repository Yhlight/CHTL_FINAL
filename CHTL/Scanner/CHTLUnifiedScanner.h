#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class FragmentType {
    CHTL,
    CHTL_JS,
    CSS,
    JS,
    UNKNOWN
};

struct CodeFragment {
    FragmentType type;
    std::string content;
};

class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner() = default;

    // Scans a file and determines its type, returning it as a single fragment.
    CodeFragment scanFile(const std::string& filepath, const std::string& source);
};

} // namespace CHTL
