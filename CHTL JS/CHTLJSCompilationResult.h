#ifndef CHTL_JS_COMPILATION_RESULT_H
#define CHTL_JS_COMPILATION_RESULT_H

#include <string>
#include <vector>
#include "CHTLJSNode/DelegateNode.h"

namespace CHTL_JS {

struct CHTLJSCompilationResult {
    std::string js;
    std::vector<DelegateNode> delegate_nodes;
};

} // namespace CHTL_JS

#endif // CHTL_JS_COMPILATION_RESULT_H
