#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>

// Helper to remove all whitespace from a string for robust comparison
std::string removeWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

int main() {
    std::string file_path = "Test/DispatcherTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    assert(!source.empty());

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scan the file
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    // 2. Dispatch the fragments
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::cout << "--- HTML ---" << std::endl;
    std::cout << result.html << std::endl;
    std::cout << "--- CSS ---" << std::endl;
    std::cout << result.css << std::endl;
    std::cout << "--- JS ---" << std::endl;
    std::cout << result.js << std::endl;

    // 1. Check for Doctype
    assert(result.html.find("<!DOCTYPE html>") != std::string::npos);

    // 2. Check for body and text content
    assert(result.html.find("<body") != std::string::npos);
    assert(result.html.find("Hello Dispatcher") != std::string::npos);

    // 3. Check for CSS content
    // This is an inline style, so it should be in the HTML style attribute.
    // Use the whitespace remover for a truly robust check.
    std::string html_no_space = removeWhitespace(result.html);
    assert(html_no_space.find("style=\"color:blue;\"") != std::string::npos);
    // The global CSS output should be empty.
    assert(result.css.empty());

    // 4. Check that JS is empty
    assert(result.js.empty());


    std::cout << "DispatcherTest passed!" << std::endl;

    return 0;
}
