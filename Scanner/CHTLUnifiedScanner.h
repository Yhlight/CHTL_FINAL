#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <variant>

namespace CHTL {

    enum class FragmentType {
        CHTL,
        CHTL_JS,
        CSS,
        JS,
        UNKNOWN
    };

    struct CodeFragment {
        std::string content;
        FragmentType type;
    };

    class CHTLUnifiedScanner {
    public:
        CHTLUnifiedScanner(const std::string& source);
        std::vector<CodeFragment> scan();

    private:
        std::string m_source;
        size_t m_cursor = 0;

        // Helper methods for scanning different parts will be needed here.
    };

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
