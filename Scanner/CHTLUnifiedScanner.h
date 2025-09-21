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
        // Used to link a placeholder to its original code
        std::string placeholder_id;
    };

    class CHTLUnifiedScanner {
    public:
        CHTLUnifiedScanner(const std::string& source);
        std::vector<CodeFragment> scan();

    private:
        std::string m_source;
        size_t m_cursor = 0;
        int m_placeholder_counter = 0;

        // Main scanning loop
        void scanSource();
        // Specific scanner for script content
        void scanScriptContent(size_t block_end);
        void scanScriptAndSubscript(size_t block_end, std::string& chtl_js_buffer);
        // Specific scanner for style content
        void scanStyleContent(size_t block_end);

        std::vector<CodeFragment> m_fragments;
    };

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
