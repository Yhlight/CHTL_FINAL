#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <vector>
#include <cassert>

struct GeneratorTestCase {
    std::string name;
    std::string input;
    std::string expectedOutput;
};

void runGeneratorTests() {
    std::cout << "--- Running Generator Tests ---" << std::endl;

    std::vector<GeneratorTestCase> testCases = {
        {
            "Basic HTML Structure",
            "html{body{p{text{\"hi\"}}}}",
            "<html><body><p>hi</p></body></html>"
        },
        {
            "Attributes",
            "a { href: \"/index.html\"; text { Click Me } }",
            "<a href=\"/index.html\">Click Me</a>"
        },
        {
            "Complex Nesting and Attributes",
            "div { id: \"main\"; article { class: card; p { text { \"Content\" } } } }",
            "<div id=\"main\"><article class=\"card\"><p>Content</p></article></div>"
        },
        {
            "HTML Escaping",
            "p { text { \"1 < 2 && 4 > 3\" } }",
            "<p>1 &lt; 2 &amp;&amp; 4 &gt; 3</p>"
        }
    };

    int passed = 0;
    for (const auto& testCase : testCases) {
        Lexer lexer(testCase.input);
        Parser parser(lexer);
        Generator generator;

        NodeList ast = parser.parse();
        std::string actualOutput = generator.generate(ast);

        if (actualOutput == testCase.expectedOutput) {
            std::cout << "[PASS] " << testCase.name << std::endl;
            passed++;
        } else {
            std::cout << "[FAIL] " << testCase.name << std::endl;
            std::cout << "  Expected: " << testCase.expectedOutput << std::endl;
            std::cout << "  Actual  : " << actualOutput << std::endl;
        }
    }

    std::cout << "Generator Tests Passed: " << passed << "/" << testCases.size() << std::endl;
    assert(passed == testCases.size());
}
