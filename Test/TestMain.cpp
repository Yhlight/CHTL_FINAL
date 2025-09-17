#include "Test.h"

int main(int argc, char* argv[]) {
    // The runAll() method returns the number of failed tests,
    // which serves as the exit code for the test program.
    // An exit code of 0 indicates success.
    return CHTL_TEST::TestRunner::getInstance().runAll();
}
