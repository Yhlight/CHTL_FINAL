#include "../../CHTL/CHTLCompiler.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../CHTL/CHTLContext.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Helper to remove all whitespace from a string
void removeWhitespace(std::string& str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

// Function to run a single test case
bool runTest(const std::string& testName, const std::string& entryFile, const std::string& expectedHtml) {
    std::cout << "--- Running Test: " << testName << " ---" << std::endl;
    std::string normalizedExpected = expectedHtml;
    removeWhitespace(normalizedExpected);

    try {
        CHTL::CHTLCompiler compiler;
        std::unique_ptr<CHTL::CHTLContext> context = compiler.compile(entryFile);

        CHTL::CHTLGenerator generator(*context);
        CHTL::CompilationResult result = generator.generate(context->files[entryFile].get());

        std::string normalizedActual = result.html;
        removeWhitespace(normalizedActual);

        if (normalizedActual == normalizedExpected) {
            std::cout << "[PASS] " << testName << std::endl;
            return true;
        } else {
            std::cout << "[FAIL] " << testName << std::endl;
            std::cout << "  Expected: " << expectedHtml << std::endl;
            std::cout << "  Actual  : " << result.html << std::endl;
            std::cout << "  (Whitespace-normalized)" << std::endl;
            std::cout << "  Expected: " << normalizedExpected << std::endl;
            std::cout << "  Actual  : " << normalizedActual << std::endl;
            return false;
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception during test " << testName << ": " << e.what() << std::endl;
        return false;
    }
}

int main() {
    int failed_tests = 0;

    // Test 1: Original test case
    if (!runTest(
        "Original Import Test",
        "Test/FullPipelineTest/test.chtl",
        R"(<div style="color:blue;font-size:16px;border:1px solid black;">This text should be blue.</div>)"
    )) {
        failed_tests++;
    }

    // Test 2: New Specialization Test
    if (!runTest(
        "Style Specialization Test",
        "Test/FullPipelineTest/specialization_test.chtl",
        R"(<body><div id="test-div" style="background-color:red;font-size:16px;font-weight:bold;"></div></body>)"
    )) {
        failed_tests++;
    }


    std::cout << "\n--- Test Summary ---" << std::endl;
    if (failed_tests == 0) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << failed_tests << " test(s) failed." << std::endl;
        return 1;
    }
}
