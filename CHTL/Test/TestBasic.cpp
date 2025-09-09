#include <iostream>
#include <string>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

using namespace CHTL;

void testLexer() {
    std::cout << "Testing CHTL Lexer..." << std::endl;
    
    std::string source = R"(
div
{
    id: "container";
    class: "main";
    
    text
    {
        Hello World
    }
}
)";
    
    CHTLLexer lexer(source);
    TokenStream tokens = lexer.tokenize();
    
    std::cout << "Tokens generated:" << std::endl;
    while (tokens.hasMore()) {
        Token token = tokens.consume();
        std::cout << "  " << token.toString() << std::endl;
    }
    
    std::cout << "Lexer test completed." << std::endl << std::endl;
}

void testParser() {
    std::cout << "Testing CHTL Parser..." << std::endl;
    
    std::string source = R"(
div
{
    id: "container";
    class: "main";
    
    text
    {
        Hello World
    }
}
)";
    
    CHTLLexer lexer(source);
    TokenStream tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    std::shared_ptr<BaseNode> ast = parser.parse();
    
    if (ast) {
        std::cout << "AST generated successfully:" << std::endl;
        std::cout << "  " << ast->toString() << std::endl;
    } else {
        std::cout << "Failed to generate AST" << std::endl;
    }
    
    std::cout << "Parser test completed." << std::endl << std::endl;
}

void testGenerator() {
    std::cout << "Testing CHTL Generator..." << std::endl;
    
    std::string source = R"(
div
{
    id: "container";
    class: "main";
    
    text
    {
        Hello World
    }
}
)";
    
    CHTLLexer lexer(source);
    TokenStream tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    std::shared_ptr<BaseNode> ast = parser.parse();
    
    if (ast) {
        CHTLGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "Generated HTML:" << std::endl;
        std::cout << output << std::endl;
    } else {
        std::cout << "Failed to generate AST" << std::endl;
    }
    
    std::cout << "Generator test completed." << std::endl << std::endl;
}

void testCompiler() {
    std::cout << "Testing CHTL Compiler..." << std::endl;
    
    std::string source = R"(
div
{
    id: "container";
    class: "main";
    
    text
    {
        Hello World
    }
}
)";
    
    CompilerDispatcher compiler;
    CompileResult result = compiler.compileString(source, "test_output.html");
    
    if (result.success) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "HTML Output:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
    } else {
        std::cout << "Compilation failed: " << result.message << std::endl;
    }
    
    std::cout << "Compiler test completed." << std::endl << std::endl;
}

int main() {
    std::cout << "CHTL Test Suite" << std::endl;
    std::cout << "===============" << std::endl << std::endl;
    
    try {
        testLexer();
        testParser();
        testGenerator();
        testCompiler();
        
        std::cout << "All tests completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}