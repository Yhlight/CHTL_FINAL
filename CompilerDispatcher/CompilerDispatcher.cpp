#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(CodeMerger& merger)
    : merger(merger) {}

void CompilerDispatcher::dispatch(const ScanResult& scan_result, const std::string& entry_point) {
    // 1. Compile the main CHTL source with placeholders
    CHTLLexer lexer(scan_result.modified_source);
    auto tokens = lexer.scanTokens();
    CHTLParser parser(scan_result.modified_source, tokens, entry_point);
    auto ast = parser.parse();

    if (!ast) {
        std::cerr << "Parsing failed, stopping dispatch." << std::endl;
        return;
    }

    CHTLGenerator generator(parser.getTemplateDefinitions());
    CompilationResult main_result = generator.generate(ast.get());

    // 2. "Compile" the extracted blocks
    for (const auto& block : scan_result.extracted_blocks) {
        if (block.type == FragmentType::CHTL_JS) {
            CHTL_JS::CHTLJSLexer cjs_lexer(block.content);
            auto cjs_tokens = cjs_lexer.scanTokens();
            CHTL_JS::CHTLJSParser cjs_parser(cjs_tokens, block.content);
            auto cjs_ast = cjs_parser.parse();
            CHTL_JS::CHTLJSGenerator cjs_generator;
            compiled_blocks[block.placeholder] = cjs_generator.generate(cjs_ast);
        } else if (block.type == FragmentType::JS) {
            // For now, just store the content as is.
            compiled_blocks[block.placeholder] = block.content;
        } else if (block.type == FragmentType::CSS) {
            // For now, just store the content as is.
            // Later, this will call the CSS compiler.
            compiled_blocks[block.placeholder] = block.content;
        }
    }

    // 3. Pass everything to the CodeMerger
    merger.setMainHtml(main_result.html);
    merger.addCss(main_result.css);
    merger.addJs(main_result.js);
    merger.setCompiledBlocks(compiled_blocks);
}

} // namespace CHTL
