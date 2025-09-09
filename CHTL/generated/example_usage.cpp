#include "chtl/CHTLLexer.h"
#include "chtl/CHTLParser.h"
#include "chtljs/CHTLJSLexer.h"
#include "chtljs/CHTLJSParser.h"
#include <iostream>
#include <fstream>

using namespace chtl::antlr;

void parseCHTL(const std::string& input) {
    antlr4::ANTLRInputStream inputStream(input);
    CHTLLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CHTLParser parser(&tokens);
    
    auto tree = parser.program();
    std::cout << "CHTL parsed successfully!" << std::endl;
}

void parseCHTLJS(const std::string& input) {
    antlr4::ANTLRInputStream inputStream(input);
    CHTLJSLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CHTLJSParser parser(&tokens);
    
    auto tree = parser.program();
    std::cout << "CHTL JS parsed successfully!" << std::endl;
}

int main() {
    std::string chtlInput = R"(
        html {
            head { title { text { Hello World } } }
            body { div { text { Hello CHTL! } } }
        }
    )";
    
    std::string chtljsInput = R"(
        const message = "Hello CHTL JS!";
        {{#myDiv}}->addEventListener('click', function() {
            console.log(message);
        });
    )";
    
    parseCHTL(chtlInput);
    parseCHTLJS(chtljsInput);
    
    return 0;
}
