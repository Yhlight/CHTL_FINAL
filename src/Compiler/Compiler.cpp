#include "Compiler.h"

#include "CHTLLexer.h"
#include "CHTLParser.h"
#include "CHTLGenerator.h"

#include <vector>
#include <memory>

std::string compile_source(const std::string& source) {
    // 1. Lexer
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 2. Parser
    CHTL::CHTLParser parser(tokens);
    std::shared_ptr<CHTL::ElementNode> ast = parser.parse();

    // 3. Generator
    CHTL::CHTLGenerator generator(ast);
    return generator.generate();
}
