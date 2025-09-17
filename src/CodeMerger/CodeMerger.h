#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CodeMerger {
public:
    explicit CodeMerger(const std::map<std::string, std::string>& placeholder_map);

    // Replaces all JS placeholders in a string with their original code.
    std::string resolveJsPlaceholders(std::string js_with_placeholders);

    // Takes the main HTML body, final CSS, and final JS
    // and merges them into a final HTML document.
    std::string merge(const std::string& html_body,
                      const std::string& css,
                      const std::string& js);
private:
    const std::map<std::string, std::string>& placeholder_map_;
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
