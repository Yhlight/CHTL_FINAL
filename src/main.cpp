#include <iostream>
#include <string>
#include "CLI/CHTLCli.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    CHTLCli cli;
    return cli.run(argc, argv);
}