#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger/CodeMerger.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>

int main() {
    std::string input_file = "Test/final_test.chtl";
    std::cout << "--- Testing file: " << input_file << " ---\n" << std::endl;

    try {
        std::string source = CHTL::FileSystem::readFile(input_file);
        std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput compiled_output = dispatcher.dispatch();

        std::cout << "\n--- Generated CSS ---\n" << compiled_output.css << "\n--------------------\n" << std::endl;
        std::cout << "\n--- Generated HTML ---\n" << compiled_output.html << "\n----------------------\n" << std::endl;

        CHTL::CodeMerger merger(compiled_output);
        std::string final_html = merger.mergeToSingleFile();

        std::cout << "\n--- Final Merged HTML ---\n" << final_html << "\n-------------------------\n" << std::endl;


    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
