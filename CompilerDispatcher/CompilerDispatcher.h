#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "../Scanner/UnifiedScanner.h"
#include <string>
#include <vector>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher();

    // Takes fragments from the scanner and returns a single, final output string.
    std::string dispatch(const std::vector<CodeFragment>& fragments);
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H
