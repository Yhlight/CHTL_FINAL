#include "CodeMerger.h"

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_body, const std::string& css_content, const std::string& js_content) {
    std::string final_html = html_body;

    if (!css_content.empty()) {
        std::string style_tag = "<style>\n" + css_content + "\n</style>\n";
        size_t head_pos = final_html.find("</head>");
        if (head_pos != std::string::npos) {
            final_html.insert(head_pos, style_tag);
        } else {
            final_html.insert(0, style_tag);
        }
    }

    if (!js_content.empty()) {
        std::string script_tag = "<script>\n" + js_content + "\n</script>\n";
        size_t body_pos = final_html.find("</body>");
        if (body_pos != std::string::npos) {
            final_html.insert(body_pos, script_tag);
        } else {
            final_html.append(script_tag);
        }
    }

    return final_html;
}

} // namespace CHTL
