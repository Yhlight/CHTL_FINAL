#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>

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
        // Specific scanner for style content
        void scanStyleContent(size_t block_end);
        // Specific scanner for script content
        void scanScriptContent(size_t block_end);

        // Helper functions
        size_t findNextKeyword(const std::string& keyword, size_t start_pos);
        size_t findMatchingBrace(size_t open_pos);
        size_t findNextCHTLFeature(size_t start_pos, size_t end_pos, const std::vector<std::string>& features);
        size_t findNextCHTLJSConstruct(size_t start_pos, size_t end_pos);
        size_t findEndOfCHTLFeature(size_t start_pos, size_t end_pos);
        size_t findEndOfCHTLJSConstruct(size_t start_pos, size_t end_pos);

        std::vector<CodeFragment> m_fragments;
    };

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H