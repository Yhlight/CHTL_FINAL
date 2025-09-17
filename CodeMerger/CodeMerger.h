#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <sstream>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger();

    // Add generated code parts to the merger.
    void addHtml(const std::string& html);
    void addCss(const std::string& css);
    void addJs(const std::string& js);

    // Assemble the final output. In later phases, this will handle
    // embedding CSS and JS into the HTML.
    std::string getFinalOutput();

private:
    std::stringstream m_html_stream;
    std::stringstream m_css_stream;
    std::stringstream m_js_stream;
};

} // namespace CHTL

#endif // CODE_MERGER_H
