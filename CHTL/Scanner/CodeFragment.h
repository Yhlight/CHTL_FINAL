#ifndef CODE_FRAGMENT_H
#define CODE_FRAGMENT_H

#include <string>
#include <vector>

namespace CHTL {

enum class FragmentType {
    CHTL,
    CSS,
    JS,
    HTML,
    CHTL_JS // Reserved for future CHTL JS implementation
};

struct CodeFragment {
    FragmentType type;
    std::string content;
    int start_line; // Keep track of original line number for better error messages
};

} // namespace CHTL

#endif // CODE_FRAGMENT_H
