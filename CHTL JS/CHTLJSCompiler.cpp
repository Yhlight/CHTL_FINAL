#include "CHTLJSCompiler.h"
#include "CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJSParser/CHTLJSParser.h"
#include "CHTLJSGenerator.h"

namespace CHTL_JS {

CHTLJSCompiler::CHTLJSCompiler() {}

CHTLJSCompilationResult CHTLJSCompiler::compile(const std::string& source) {
    CHTLJSLexer lexer(source);
    auto tokens = lexer.scanTokens();

    CHTLJSParser parser(tokens, source);
    auto ast = parser.parse();

    CHTLJSGenerator generator;
    return generator.generate(ast);
}

} // namespace CHTL_JS
