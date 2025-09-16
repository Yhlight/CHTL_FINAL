#include "CJMOD/API/CJMODManager.h"
#include "CJMOD/API/Syntax.h"
#include <iostream>

// This is the function that will be called by the CHTL compiler when this module is loaded.
extern "C" void chtl_cjmod_init(std::shared_ptr<CHTL::CJMOD::CJMODManager> manager) {
    std::cout << "[printMylove.cjmod] Initializing..." << std::endl;

    // Define the syntax for the printMylove function.
    // Example: printMylove { url: "...", mode: ASCII }
    CHTL::CJMOD::Arg rule = CHTL::CJMOD::Syntax::analyze("{ url: $!, mode: $? }");

    // Define the transformation rule.
    // This is a simplified example; a real implementation would be more complex.
    rule.transform("console.log('Printing image from ' + url + ' in ' + mode + ' mode.')");

    // Register the rule with the manager.
    manager->registerRule("printMylove", rule);
}
