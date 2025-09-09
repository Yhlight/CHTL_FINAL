#include "CHTLJSCompiler.h"
#include "CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJSParser/CHTLJSParser.h"
#include "CHTLJSGenerator/CHTLJSGenerator.h"

namespace CHTL {
namespace CHTL_JS {

CHTLJSCompiler::CHTLJSCompiler() {}

std::string CHTLJSCompiler::compile(const std::string& source) {
    CHTLJSLexer lexer(source);
    CHTLJSParser parser(lexer);
    auto program = parser.parseProgram();

    // In a real implementation, we would check for parser errors here.

    CHTLJSGenerator generator;
    return generator.generate(program.get());
}

} // namespace CHTL_JS
} // namespace CHTL
