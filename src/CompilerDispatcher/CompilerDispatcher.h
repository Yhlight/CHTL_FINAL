#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "../Scanner/CodeFragment.h"
#include <string>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher() = default;
    std::string compile(const FragmentList& fragments, const std::string& initial_path);
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
