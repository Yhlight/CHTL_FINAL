#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "CHTL/CHTLParser/ParserContext.h"
#include <string>
#include <memory>

namespace CHTL {

// Forward declarations
class CHTLGenerator;
class CHTLLoader;

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    std::string compile(const std::string& source);

private:
    std::shared_ptr<ParserContext> chtl_context_;
    std::unique_ptr<CHTLLoader> chtl_loader_;
    std::unique_ptr<CHTLGenerator> chtl_generator_;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
