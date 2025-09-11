#ifndef CHTL_FRAGMENT_H
#define CHTL_FRAGMENT_H

#include <string>
#include <vector>

// Enum to identify the type of code in a fragment.
enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS, // Reserved for future use
    UNKNOWN
};

// Struct to hold a fragment of code and its type.
struct CodeFragment {
    std::string code;
    FragmentType type;
    // Line and column numbers can be added later for error reporting.
};

#endif // CHTL_FRAGMENT_H
