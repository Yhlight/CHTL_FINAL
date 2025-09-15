#include "CodeMerger/CodeMerger.h"
#include <sstream>
#include <map>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output,
                              const std::map<std::string, std::string>& scripts) {

    if (scripts.empty()) {
        return html_output;
    }

    std::string final_html = html_output;

    // The CHTL Generator produces HTML with placeholders (e.g., <script>__CHTL_SCRIPT_PLACEHOLDER_1__</script>).
    // The merger's job is to replace these placeholders with the actual script content.
    // This new logic respects the original position of the script blocks.

    for (const auto& pair : scripts) {
        const std::string& placeholder = pair.first;
        const std::string& script_content = pair.second;

        size_t pos = final_html.find(placeholder);
        if (pos != std::string::npos) {
            final_html.replace(pos, placeholder.length(), script_content);
        }
    }

    return final_html;
}

} // namespace CHTL
