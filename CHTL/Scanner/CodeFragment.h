#ifndef CODE_FRAGMENT_H
#define CODE_FRAGMENT_H

#include <string>
#include <vector>

namespace CHTL {

enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS,
    CHTL_in_CSS // CHTL syntax found inside a global style block
};

#include <map>

struct CodeFragment {
    FragmentType type;
    std::string content;
    int start_line; // Keep track of original line number for better error messages

    // For CHTL_JS fragments, this holds the pure JS parts that were extracted.
    std::map<std::string, std::string> js_placeholders;
};

} // namespace CHTL

#endif // CODE_FRAGMENT_H
