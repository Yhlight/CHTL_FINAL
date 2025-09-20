#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger/CodeMerger.h"
#include <iostream>
#include <string>

int main() {
    std::string source = R"(
        div {
            style {
                width: $boxWidth$;
                height: 100px;
            }
            text { "Reactive value test" }
        }
    )";

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    try {
        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput compiled_output = dispatcher.dispatch();

        std::cout << "\n--- Merging ---" << std::endl;
        CHTL::CodeMerger merger(compiled_output);
        std::string final_html = merger.mergeToSingleFile();
        std::cout << "Merging complete." << std::endl;

        std::cout << "\n--- Final Merged HTML ---\n" << final_html << "\n-------------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
