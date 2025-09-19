#ifndef CHTL_COMPILATION_RESULT_H
#define CHTL_COMPILATION_RESULT_H

#include <string>

namespace CHTL {

struct CompilationResult {
    std::string html;
    std::string css;
    std::string js;
};

} // namespace CHTL

#endif // CHTL_COMPILATION_RESULT_H
