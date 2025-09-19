#include "CodeMerger.h"
#include <string>
#include <iostream>

namespace CHTL {

    std::string CodeMerger::mergeJavaScript(
        const std::string& js_with_placeholders,
        const std::map<std::string, std::string>& js_fragments
    ) {
        std::string result = js_with_placeholders;

        for(auto const& [id, content] : js_fragments) {
            size_t pos = result.find(id);
            if (pos != std::string::npos) {
                result.replace(pos, id.length(), content);
            }
        }

        return result;
    }

    std::string CodeMerger::mergeToFinalHtml(const std::string& html, const std::string& css, const std::string& js) {
        std::string finalHtml = html;

        // Embed CSS if it exists
        if (!css.empty()) {
            std::string styleTag = "<style>\n" + css + "\n</style>";
            size_t head_pos = finalHtml.find("</head>");
            if (head_pos != std::string::npos) {
                finalHtml.insert(head_pos, styleTag);
            } else {
                finalHtml += styleTag;
            }
        }

        // Embed JS if it exists
        if (!js.empty()) {
            std::string scriptTag = "<script>\n" + js + "\n</script>";
            size_t body_pos = finalHtml.find("</body>");
            if (body_pos != std::string::npos) {
                finalHtml.insert(body_pos, scriptTag);
            } else {
                finalHtml += scriptTag;
            }
        }

        return finalHtml;
    }

} // namespace CHTL
