#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <map>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger() = default;

    // Takes a string containing placeholders and a map, and returns the final string.
    std::string substitutePlaceholders(
        const std::string& js_with_placeholders,
        const std::map<std::string, std::string>& placeholder_map
    );
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
