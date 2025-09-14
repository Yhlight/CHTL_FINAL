#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
// CHTLJS and Scanner includes are no longer needed here, they will be used by the CHTLGenerator

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    // Initialize contexts
    chtl_context_ = std::make_shared<ParserContext>();
    // chtljs_context_ will be created and used by the CHTLGenerator

    // Initialize components
    chtl_loader_ = std::make_unique<CHTLLoader>();
    chtl_generator_ = std::make_unique<CHTLGenerator>();
    // Other components are no longer needed at this top level
}

// Destructor must be defined here where the full types are known
CompilerDispatcher::~CompilerDispatcher() = default;

std::string CompilerDispatcher::compile(const std::string& source) {
    // The new, correct pipeline:
    // 1. Lex the entire source file.
    CHTLLexer lexer(source);
    auto tokens = lexer.scanTokens();
    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
        return "";
    }

    // 2. Parse the entire token stream into a single AST.
    // The parser is still stateful with chunk data, so it's created here.
    CHTLParser parser(source, tokens, *chtl_loader_, "./", chtl_context_);
    std::unique_ptr<RootNode> ast = parser.parse();

    // 3. Generate the final output from the AST.
    // The generator will be responsible for handling style/script blocks internally.
    CompilationResult result = chtl_generator_->generate(*ast);

    // For now, we assume the generator's HTML output is the final product.
    // The js and css parts of CompilationResult will be populated by the generator
    // in a later step. A CodeMerger might be needed inside the generator or not at all.
    // For this refactoring step, we only return the HTML part.
    return result.html;
}

} // namespace CHTL
