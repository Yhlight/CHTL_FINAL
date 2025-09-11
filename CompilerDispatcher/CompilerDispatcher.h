#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "../Util/Fragment.h"
#include <vector>

class CompilerDispatcher {
public:
    /**
     * @brief Dispatches code fragments to their respective compilers.
     * @param fragments A vector of CodeFragment objects from the scanner.
     */
    void dispatch(const std::vector<CodeFragment>& fragments);
};

#endif // CHTL_COMPILER_DISPATCHER_H
