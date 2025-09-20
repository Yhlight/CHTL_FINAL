#ifndef CODE_FRAGMENT_H
#define CODE_FRAGMENT_H

#include <string>
#include <vector>

namespace CHTL {

enum class FragmentType {
    // Raw fragments from the scanner
    CHTL,
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    IMPORT_STATEMENT,
    // Processed fragments
    CSS,
    JS,
    CHTL_JS,
    CHTL_in_CSS
};

struct CodeFragment {
    FragmentType type;
    std::string content;
    int start_line;
};

} // namespace CHTL

#endif // CODE_FRAGMENT_H
