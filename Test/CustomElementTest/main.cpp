#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>

// Helper function to remove all whitespace from a string for robust comparison
std::string remove_whitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            out += c;
        }
    }
    return out;
}

int main() {
    std::string file_path = "Test/CustomElementTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    std::vector<CHTL::CodeFragment> fragments = {{source, CHTL::FragmentType::CHTL}};

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::string expected_html = R"(
        <body>
            <header>
                <h1>Default Title</h1>
            </header>
            <section>
                <p>Default paragraph 1.</p>
            </section>
            <footer>Article Footer</footer>
        </body>
    )";

    std::cout << "Generated HTML: " << result.html << std::endl;

    assert(remove_whitespace(result.html) == remove_whitespace(expected_html));

    std::cout << "CustomElementTest PASSED!" << std::endl;

    return 0;
}
