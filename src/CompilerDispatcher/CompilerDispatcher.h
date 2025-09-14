#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "../CHTL/CHTLParser/ParserContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher();

    std::string compile(const std::string& source);

private:
    std::shared_ptr<ParserContext> chtl_context_;
    // Other contexts like CHTLJSContext can be added here later
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
