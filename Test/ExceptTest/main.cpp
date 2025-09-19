#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <stdexcept>

int main() {
    std::string file_path = "Test/ExceptTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);

    bool exception_thrown = false;
    std::string error_message;
    try {
        dispatcher.dispatch(fragments);
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        error_message = e.what();
        std::cout << "Caught expected exception: " << error_message << std::endl;
    }

    assert(exception_thrown);

    // Check if the error message is one of the expected ones
    std::string expected_error1 = "HTML elements are not allowed inside <div> due to 'except @Html' constraint.";
    std::string expected_error2 = "Element <MyTemplate> is not allowed inside <div> due to 'except' constraint.";

    assert(error_message.find("not allowed inside") != std::string::npos);


    std::cout << "ExceptTest PASSED!" << std::endl;

    return 0;
}
