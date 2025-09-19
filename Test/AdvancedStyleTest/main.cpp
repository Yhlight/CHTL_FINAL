#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// A simple utility to remove all whitespace from a string for comparison
std::string removeWhitespace(const std::string& str) {
    std::string out;
    out.reserve(str.length());
    for (char c : str) {
        if (!isspace(c)) {
            out += c;
        }
    }
    return out;
}

struct TestCase {
    std::string name;
    std::string input;
    std::string expected_html;
    std::string expected_css;
};

bool runTest(const TestCase& test) {
    std::cout << "--- Running Test: " << test.name << " ---" << std::endl;
    bool success = true;
    std::string generated_html;
    std::string generated_css;

    try {
        CHTL::CHTLLexer lexer(test.input);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(test.input, tokens);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        if (ast) {
            CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
            CHTL::CompilationResult result = generator.generate(ast.get());
            generated_html = result.html;
            generated_css = result.css;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "!!! TEST FAILED with exception: " << e.what() << std::endl;
        success = false;
    }

    std::string clean_gen_html = removeWhitespace(generated_html);
    std::string clean_exp_html = removeWhitespace(test.expected_html);
    std::string clean_gen_css = removeWhitespace(generated_css);
    std::string clean_exp_css = removeWhitespace(test.expected_css);

    if (success && clean_gen_html != clean_exp_html) {
        std::cerr << "!!! HTML TEST FAILED: Output mismatch!" << std::endl;
        std::cerr << "Expected: " << test.expected_html << std::endl;
        std::cerr << "Got:      " << generated_html << std::endl;
        success = false;
    }

    if (success && clean_gen_css != clean_exp_css) {
        std::cerr << "!!! CSS TEST FAILED: Output mismatch!" << std::endl;
        std::cerr << "Expected: " << test.expected_css << std::endl;
        std::cerr << "Got:      " << generated_css << std::endl;
        success = false;
    }

    if (success) {
        std::cout << ">>> TEST PASSED <<<" << std::endl;
    }

    std::cout << "--------------------------------------" << std::endl << std::endl;
    return success;
}

int main() {
    std::vector<TestCase> tests = {
        {
            "Auto-add single class",
            R"(div{ style{ .box{color:red;} } })",
            R"(<div class="box"></div>)",
            R"(.box { color: red; })"
        },
        {
            "Append to existing class",
            R"(div{ class: container; style{ .box{color:red;} } })",
            R"(<div class="container box"></div>)",
            R"(.box { color: red; })"
        },
        {
            "Do not add duplicate class",
            R"(div{ class: box; style{ .box{color:red;} } })",
            R"(<div class="box"></div>)",
            R"(.box { color: red; })"
        },
        {
            "Auto-add single ID",
            R"(div{ style{ #main{color:red;} } })",
            R"(<div id="main"></div>)",
            R"(#main { color: red; })"
        },
        {
            "Do not add ID if one exists",
            R"(div{ id: first; style{ #main{color:red;} } })",
            R"(<div id="first"></div>)",
            R"(#main { color: red; })"
        },
        {
            "Add only first ID from style block",
            R"(div{ style{ #main{color:red;} #secondary{color:blue;} } })",
            R"(<div id="main"></div>)",
            R"(#main { color: red; } #secondary { color: blue; })"
        },
        {
            "& selector with class",
            R"(div{ class: box; style{ &:hover{color:red;} } })",
            R"(<div class="box"></div>)",
            R"(.box:hover { color: red; })"
        },
        {
            "& selector with id",
            R"(div{ id: main; style{ &:hover{color:red;} } })",
            R"(<div id="main"></div>)",
            R"(#main:hover { color: red; })"
        },
        {
            "& selector with tag name fallback",
            R"(div{ style{ &:hover{color:red;} } })",
            R"(<div></div>)",
            R"(div:hover { color: red; })"
        },
        {
            "& selector with auto-generated class",
            R"(div{ style{ .box{color:blue;} &:hover{color:red;} } })",
            R"(<div class="box"></div>)",
            R"(.box { color: blue; } .box:hover { color: red; })"
        },
        {
            "Reference property from ID selector",
            R"(
            body {
                div {
                    id: main;
                    style{ width: 100px; }
                }
                p {
                    style{ width: #main.width; }
                }
            }
            )",
            R"(<body><div id="main" style="width:100.000000px;"></div><p style="width:100.000000px;"></p></body>)",
            ""
        },
        {
            "Reference property from Class selector",
            R"(
            body {
                div {
                    class: box;
                    style{ height: 50px; }
                }
                p {
                    style{ height: .box.height; }
                }
            }
            )",
            R"(<body><div class="box" style="height:50.000000px;"></div><p style="height:50.000000px;"></p></body>)",
            ""
        },
        {
            "Calculate with referenced property",
            R"(
            body {
                div {
                    id: main;
                    style{ width: 100px; }
                }
                p {
                    style{ width: #main.width / 2; }
                }
            }
            )",
            R"(<body><div id="main" style="width:100.000000px;"></div><p style="width:50.000000px;"></p></body>)",
            ""
        }
    };

    int passed_count = 0;
    for (const auto& test : tests) {
        if (runTest(test)) {
            passed_count++;
        }
    }

    std::cout << "======================================" << std::endl;
    std::cout << "Advanced Style Test Summary: " << passed_count << " / " << tests.size() << " passed." << std::endl;
    std::cout << "======================================" << std::endl;

    return (passed_count == tests.size()) ? 0 : 1;
}
