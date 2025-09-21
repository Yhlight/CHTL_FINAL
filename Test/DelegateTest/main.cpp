#include "../../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <vector>
#include <memory>

// Helper function to remove all whitespace for comparison
std::string normalize_code(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!isspace(c)) {
            result += c;
        }
    }
    return result;
}


int main() {
    // Read the CHTL JS content directly. No more fragile string extraction.
    std::string script_content = CHTL::FileSystem::readFile("Test/DelegateTest/main.chtl");

    CHTL_JS::CHTLJSLexer lexer(script_content);
    auto tokens = lexer.scanTokens();

    CHTL_JS::CHTLJSParser parser(tokens, script_content);
    std::vector<std::unique_ptr<CHTL_JS::CHTLJSBaseNode>> ast = parser.parse();

    CHTL_JS::CHTLJSGenerator generator;
    std::string js_code = generator.generate(ast);

    std::cout << "--- Generated JS Code ---\n" << js_code << "\n-------------------------\n";

    std::string expected_js = R"EOF(
(function() {
    if (!window.__chtl_delegate_registry) {
        window.__chtl_delegate_registry = {};
    }
})();
if (!window.__chtl_delegate_registry['#parent-container']) {
    window.__chtl_delegate_registry['#parent-container'] = { handlers: {}, element: document.querySelector('#parent-container') };
}
if (!window.__chtl_delegate_registry['#parent-container'].handlers['click']) {
    window.__chtl_delegate_registry['#parent-container'].handlers['click'] = {};
    window.__chtl_delegate_registry['#parent-container'].element.addEventListener('click', function(e) {
        const handlers = window.__chtl_delegate_registry['#parent-container'].handlers['click'];
        for (const targetSelector in handlers) {
            if (e.target.matches(targetSelector)) {
                handlers[targetSelector].call(e.target, e);
            }
        }
    });
}
window.__chtl_delegate_registry['#parent-container'].handlers['click']['.child-button'] = (e) => { console.log('Button clicked:', e.target.textContent); };
if (!window.__chtl_delegate_registry['#parent-container']) {
    window.__chtl_delegate_registry['#parent-container'] = { handlers: {}, element: document.querySelector('#parent-container') };
}
if (!window.__chtl_delegate_registry['#parent-container'].handlers['mouseover']) {
    window.__chtl_delegate_registry['#parent-container'].handlers['mouseover'] = {};
    window.__chtl_delegate_registry['#parent-container'].element.addEventListener('mouseover', function(e) {
        const handlers = window.__chtl_delegate_registry['#parent-container'].handlers['mouseover'];
        for (const targetSelector in handlers) {
            if (e.target.matches(targetSelector)) {
                handlers[targetSelector].call(e.target, e);
            }
        }
    });
}
window.__chtl_delegate_registry['#parent-container'].handlers['mouseover']['.child-span'] = (e) => { e.target.style.color = 'red'; };
)EOF";

    std::string normalized_actual = normalize_code(js_code);
    std::string normalized_expected = normalize_code(expected_js);

    assert(normalized_actual == normalized_expected);

    std::cout << "DelegateTest Passed!" << std::endl;

    return 0;
}
