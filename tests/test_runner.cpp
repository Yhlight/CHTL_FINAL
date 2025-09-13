#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

namespace fs = std::filesystem;

// Function to execute a command and capture its combined stdout and stderr
std::string exec(const std::string& cmd_str) {
    std::string cmd = cmd_str + " 2>&1";
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main() {
    // NOTE: Tests are temporarily disabled as per user instruction
    // because the project is in a non-runnable state during major refactoring.
    std::cout << "Tests are temporarily disabled." << std::endl;
    return 0;
}
