#include <iostream>
#include <string>
#include "CHTL.h"

int main() {
    std::cout << "CHTL Test Suite" << std::endl;
    std::cout << "===============" << std::endl;
    
    // Test basic compilation
    CHTL::CHTLCompiler compiler;
    
    // Test input
    std::string testInput = R"(html
{
    head
    {
        title
        {
            "CHTL Test Page"
        }
    }
    
    body
    {
        div
        {
            class: container;
            text
            {
                Hello, CHTL World!
            }
        }
        
        // This is a line comment
        /* This is a block comment */
        -- This is a generator comment
        
        style
        {
            .container
            {
                width: 100%;
                height: 100vh;
                background-color: #f0f0f0;
            }
        }
    }
})";
    
    std::cout << "Testing CHTL compilation..." << std::endl;
    
    std::string output;
    bool success = compiler.compileString(testInput, output);
    
    if (success) {
        std::cout << "✓ Compilation successful!" << std::endl;
        std::cout << "Output length: " << output.length() << " characters" << std::endl;
        
        auto warnings = compiler.getWarnings();
        if (!warnings.empty()) {
            std::cout << "Warnings:" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "  - " << warning << std::endl;
            }
        }
        
        // Write test output
        std::ofstream testFile("test_output.html");
        if (testFile.is_open()) {
            testFile << output;
            testFile.close();
            std::cout << "✓ Test output written to test_output.html" << std::endl;
        }
        
    } else {
        std::cout << "✗ Compilation failed!" << std::endl;
        
        auto errors = compiler.getErrors();
        for (const auto& error : errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    // Test lexer directly
    std::cout << "\nTesting CHTL Lexer..." << std::endl;
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize("div { class: test; text { Hello } }");
    
    std::cout << "✓ Tokenized " << tokens.size() << " tokens" << std::endl;
    
    auto lexerErrors = lexer.getErrors();
    if (!lexerErrors.empty()) {
        std::cout << "Lexer errors:" << std::endl;
        for (const auto& error : lexerErrors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "\nTest suite completed!" << std::endl;
    return 0;
}