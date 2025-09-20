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
    CHTL_in_CSS,
    JS_PLACEHOLDER // A pure JS fragment that was extracted from a script block
};

struct CodeFragment {
    FragmentType type;
    std::string content;
    int start_line;
    std::string placeholder_id; // Used only when type is JS_PLACEHOLDER

    // Default constructor
    CodeFragment(FragmentType t, std::string c, int line, std::string id = "")
        : type(t), content(std::move(c)), start_line(line), placeholder_id(std::move(id)) {}
};

} // namespace CHTL

#endif // CODE_FRAGMENT_H
