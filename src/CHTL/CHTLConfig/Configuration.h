#ifndef CHTL_CONFIGURATION_H
#define CHTL_CONFIGURATION_H

#include <string>
#include <cstddef> // For size_t

namespace CHTL {

struct Configuration {
    // Default values as specified or inferred from CHTL.md
    size_t INDEX_INITIAL_COUNT = 0;
    bool DEBUG_MODE = false;
    bool DISABLE_STYLE_AUTO_ADD_CLASS = false;
    bool DISABLE_STYLE_AUTO_ADD_ID = false;
    bool DISABLE_DEFAULT_NAMESPACE = false;
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_H
