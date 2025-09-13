#ifndef CHTL_CODE_FRAGMENT_H
#define CHTL_CODE_FRAGMENT_H

#include <string>
#include <vector>

namespace CHTL {

enum class FragmentType {
    CHTL,
    JavaScript,
    CSS
};

struct CodeFragment {
    std::string content;
    FragmentType type;
};

using FragmentList = std::vector<CodeFragment>;

} // namespace CHTL

#endif // CHTL_CODE_FRAGMENT_H
