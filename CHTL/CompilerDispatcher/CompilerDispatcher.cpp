#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <vector>
#include <map>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string initial_source)
    : initial_source(std::move(initial_source)) {}

FinalOutput CompilerDispatcher::dispatch() {
    std::cout << "--- Running Unified Scanner ---" << std::endl;
    CHTLUnifiedScanner scanner(this->initial_source);
    std::vector<CodeFragment> fragments = scanner.scan();
    const auto& placeholder_map = scanner.getPlaceholderMap();
    std::cout << "Scanner found " << fragments.size() << " fragments and " << placeholder_map.size() << " placeholders." << std::endl;

    // In a real implementation, we would now process the placeholders.
    // For now, we will just dispatch the fragments to their respective outputs.

    std::string chtl_source_for_generation;

    for (const auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL:
                // For now, we'll just concatenate all CHTL parts for the final generation pass.
                // A more complex system would handle them more intelligently.
                chtl_source_for_generation += fragment.content;
                break;
            case FragmentType::CSS:
                final_output.css += fragment.content;
                break;
            case FragmentType::JS:
                // This JS still contains CHTL-JS and placeholders.
                // A full implementation would send this to a CHTL-JS compiler.
                final_output.js += fragment.content;
                break;
            case FragmentType::CHTL_JS:
                // The current scanner design inlines CHTL_JS within JS fragments,
                // so this case might not be used directly.
                final_output.js += fragment.content;
                break;
        }
    }

    std::cout << "\n--- Generating from CHTL fragments ---" << std::endl;
    if (!chtl_source_for_generation.empty()) {
        CHTLLexer final_lexer(chtl_source_for_generation);
        auto final_tokens = final_lexer.scanTokens();
        CHTLParser final_parser(chtl_source_for_generation, final_tokens, this->all_template_definitions);
        auto main_ast = final_parser.parse();

        CHTLGenerator generator(this->all_template_definitions);
        // We assume the generator now returns a struct with only html and css,
        // as the js part is handled separately.
        auto result = generator.generate(main_ast.get());
        this->final_output.html = result.html;
        // The generator might also produce some CSS from style attributes etc.
        this->final_output.css += result.css;
    }

    std::cout << "--- Dispatch complete ---" << std::endl;
    return final_output;
}

} // namespace CHTL
