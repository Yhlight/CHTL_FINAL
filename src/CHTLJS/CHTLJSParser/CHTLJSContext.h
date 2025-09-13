#ifndef CHTL_JS_CONTEXT_H
#define CHTL_JS_CONTEXT_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <memory>
#include <map>

namespace CHTLJS {

struct CHTLJSContext {
    // Stores the mapping from a virtual object's name to the AST node it represents.
    std::map<std::string, std::shared_ptr<CHTLJSNode>> virtual_objects;
};

} // namespace CHTLJS

#endif // CHTL_JS_CONTEXT_H
