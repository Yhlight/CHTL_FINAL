#ifndef CHTL_JS_COMPILER_H
#define CHTL_JS_COMPILER_H

#include <string>
#include "CHTLJSCompilationResult.h"

namespace CHTL_JS {

class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    CHTLJSCompilationResult compile(const std::string& source);
};

} // namespace CHTL_JS

#endif // CHTL_JS_COMPILER_H
