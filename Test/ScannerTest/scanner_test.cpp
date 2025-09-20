#include "CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

void print_fragment(const CHTL::CodeFragment& fragment) {
    std::string type;
    switch (fragment.type) {
        case CHTL::FragmentType::CHTL: type = "CHTL"; break;
        case CHTL::FragmentType::CSS: type = "CSS"; break;
        case CHTL::FragmentType::JS: type = "JS"; break;
        case CHTL::FragmentType::CHTL_JS: type = "CHTL_JS"; break;
    }
    std::cout << "--- Fragment ---" << std::endl;
    std::cout << "Type: " << type << std::endl;
    std::cout << "Line: " << fragment.start_line << std::endl;
    std::cout << "Content: \n" << fragment.content << std::endl;
    std::cout << "----------------" << std::endl;
}

int main() {
    std::string source = R"CHT(
div {
    text: "This is some CHTL code";
}

style {
    body {
        background-color: #eee;
    }
}

p {
    text: "More CHTL";
}

script {
    const a = 1;
    let b = 2;
    console.log(a + b);

    const myElement = {{#my-id}};

    myElement->Listen {
        click: () => {
            console.log("Clicked!");
            const nested = {{.nested-class}};
        }
    };

    function regularJsFunc(p) {
        return p * 2;
    }
}
    )CHT";

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeFragment> fragments = scanner.scan();

    for (const auto& frag : fragments) {
        print_fragment(frag);
    }

    // --- Comprehensive Assertions ---
    assert(fragments.size() == 8);

    // Fragment 0: CHTL
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content.find("div {") != std::string::npos);

    // Fragment 1: CSS
    assert(fragments[1].type == CHTL::FragmentType::CSS);
    assert(fragments[1].content.find("background-color") != std::string::npos);

    // Fragment 2: CHTL
    assert(fragments[2].type == CHTL::FragmentType::CHTL);
    assert(fragments[2].content.find("p {") != std::string::npos);

    // --- Script Fragments ---
    // Fragment 3: First JS chunk
    assert(fragments[3].type == CHTL::FragmentType::JS);
    assert(fragments[3].content.find("const myElement =") != std::string::npos);

    // Fragment 4: JS after first CHTL JS token
    assert(fragments[4].type == CHTL::FragmentType::JS);
    assert(fragments[4].content.find(";") != std::string::npos);
    assert(fragments[4].content.find("myElement") != std::string::npos);

    // Fragment 5: JS before second CHTL JS token
    assert(fragments[5].type == CHTL::FragmentType::JS);
    assert(fragments[5].content.find("const nested =") != std::string::npos);

    // Fragment 6: Final JS chunk
    assert(fragments[6].type == CHTL::FragmentType::JS);
    assert(fragments[6].content.find("function regularJsFunc") != std::string::npos);

    // Fragment 7: The structured CHTL_JS fragment
    assert(fragments[7].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[7].content.find("const myElement = _JS_CODE_PLACEHOLDER_{{#my-id}}") != std::string::npos);
    assert(fragments[7].content.find("myElement_JS_CODE_PLACEHOLDER_->Listen") != std::string::npos);
    assert(fragments[7].content.find("const nested = _JS_CODE_PLACEHOLDER_{{.nested-class}}") != std::string::npos);


    std::cout << "\n\nComprehensive Scanner Test PASSED!" << std::endl;

    return 0;
}
