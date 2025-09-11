#include "CompilerDispatcher.h"
#include <iostream>

// Include the new compiler component headers
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"
#include "ASTProcessors/StyleProcessor.h"

// --- CHTL Compiler Invocation ---
void invoke_chtl_compiler(const std::string& code) {
    std::cout << "--- CHTL Compiler Invoked ---" << std::endl;

    // 1. Lexer
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();

    // --- Lexer Debug Print (optional) ---
    // for (const auto& token : tokens) {
    //     std::cout << "Token: " << tokenTypeToString(token.type) << ", Lexeme: '" << token.lexeme << "'" << std::endl;
    // }

    // 2. Parser
    Parser parser(code, tokens);
    std::shared_ptr<BaseNode> ast_root = parser.parse();

    if (parser.hasError() || !ast_root) {
        std::cerr << "Compilation failed during parsing." << std::endl;
        std::cout << "-----------------------------" << std::endl;
        return;
    }

    // 3. AST Processing
    StyleProcessor styleProcessor;
    styleProcessor.process(ast_root);

    // 4. Generator
    Generator generator;
    std::string html_output = generator.generate(ast_root);

    std::cout << "--- Generated HTML ---" << std::endl;
    std::cout << html_output;
    std::cout << "----------------------" << std::endl;
}

// --- Placeholder "Compiler" Functions for other languages ---
void invoke_css_compiler(const std::string& code) {
    std::cout << "--- CSS Compiler Invoked (Placeholder) ----" << std::endl;
    std::cout << code << std::endl;
    std::cout << "------------------------------------------" << std::endl;
}

void invoke_js_compiler(const std::string& code) {
    std::cout << "--- JS Compiler Invoked (Placeholder) -----" << std::endl;
    std::cout << code << std::endl;
    std::cout << "------------------------------------------" << std::endl;
}

// --- CompilerDispatcher Implementation ---

void CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL:
                invoke_chtl_compiler(fragment.code);
                break;
            case FragmentType::CSS:
                invoke_css_compiler(fragment.code);
                break;
            case FragmentType::JS:
                invoke_js_compiler(fragment.code);
                break;
            case FragmentType::CHTL_JS:
                std::cout << "--- CHTL_JS Fragment (Not Implemented) ---" << std::endl;
                break;
            case FragmentType::UNKNOWN:
                std::cout << "--- Unknown Fragment Type ---" << std::endl;
                break;
        }
    }
}
