#ifndef CHTL_JS_CONTEXT_H
#define CHTL_JS_CONTEXT_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <memory>
#include <map>

namespace CHTLJS {

struct CHTLJSContext {
    // Stores a non-owning raw pointer from a virtual object's name to the AST node it represents.
    // The main AST owns the node via std::unique_ptr.
    std::map<std::string, CHTLJSNode*> virtual_objects;
};

} // namespace CHTLJS

#endif // CHTL_JS_CONTEXT_H
