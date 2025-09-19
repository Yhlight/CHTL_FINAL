#include "../../Scanner/UnifiedScanner.h"
#include "../../CompilerDispatcher/CompilerDispatcher.h"
#include "../../CodeMerger/CodeMerger.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    std::string entry_point = "Test/ArchitectureTest/test.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL from " << entry_point << " ---\n" << source << "\n------------------\n" << std::endl;

    try {
        // 1. Scan with UnifiedScanner
        CHTL::UnifiedScanner scanner(source);
        CHTL::ScanResult scan_result = scanner.scan();

        // 2. Dispatch and compile
        CHTL::CodeMerger merger;
        CHTL::CompilerDispatcher dispatcher(merger);
        dispatcher.dispatch(scan_result, entry_point);

        // 3. Get final result from merger
        CHTL::CompilationResult final_result = merger.getResult();

        std::cout << "--- Final Generated HTML ---\n" << final_result.html << "\n----------------------------\n" << std::endl;
        std::cout << "--- Final Generated JS ---\n" << final_result.js << "\n--------------------------\n" << std::endl;


    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
