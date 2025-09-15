#include "CLI/CHTLCli.h"
#include <iostream>

int main(int argc, char* argv[]) {
    CHTL::CHTLCli cli;
    return cli.run(argc, argv);
}