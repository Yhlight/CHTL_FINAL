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
    std::cout << "Running CHTL Snapshot Tester..." << std::endl;
    int failed_tests = 0;

    fs::path tests_dir("tests");
    fs::path snapshots_dir("tests/snapshots");
    fs::path compiler_path("chtl_compiler"); // Assumes compiler is in the same dir or in PATH

    if (!fs::exists(compiler_path)) {
        compiler_path = "./build/chtl_compiler";
         if (!fs::exists(compiler_path)) {
            std::cerr << "FATAL: Compiler executable not found at " << compiler_path << std::endl;
            return 1;
         }
    }

    for (const auto& entry : fs::directory_iterator(tests_dir)) {
        if (entry.path().extension() == ".chtl") {
            const std::string test_name = entry.path().stem().string();
            // Skip test assets
            if (test_name.rfind("ns", 0) == 0 || test_name.rfind("namespace_test", 0) == 0 || test_name == "scanner_js_test") continue;

            std::cout << "TEST: " << test_name << " ... ";

            fs::path snapshot_path = snapshots_dir / (entry.path().filename().string() + ".snap");

            std::string command = compiler_path.string() + " " + entry.path().string();
            std::string actual_output = exec(command);

            if (actual_output.find("Error") != std::string::npos) {
                std::cout << "FAILED (Compiler Error)" << std::endl;
                std::cout << actual_output;
                failed_tests++;
                continue;
            }

            if (!fs::exists(snapshot_path)) {
                std::cout << "WARNING: No snapshot found. Creating one." << std::endl;
                std::ofstream snapshot_file(snapshot_path);
                snapshot_file << actual_output;
                snapshot_file.close();
            } else {
                std::ifstream snapshot_file(snapshot_path);
                std::stringstream buffer;
                buffer << snapshot_file.rdbuf();
                std::string expected_output = buffer.str();
                snapshot_file.close();

                if (actual_output == expected_output) {
                    std::cout << "PASSED" << std::endl;
                } else {
                    std::cout << "FAILED" << std::endl;
                    std::cout << "--- EXPECTED ---\n" << expected_output << "\n";
                    std::cout << "--- ACTUAL ---\n" << actual_output << "\n";
                    std::cout << "--- DIFF ---\n";
                    // A proper diff would be better, but this is a start
                    std::string diff_command = "diff -u " + snapshot_path.string() + " -";
                    FILE* pipe = popen(diff_command.c_str(), "w");
                    fwrite(actual_output.c_str(), 1, actual_output.size(), pipe);
                    pclose(pipe);
                    failed_tests++;
                }
            }
        }
    }

    if (failed_tests > 0) {
        std::cout << "\n" << failed_tests << " test(s) failed." << std::endl;
        return 1;
    }

    std::cout << "\nAll tests passed." << std::endl;
    return 0;
}
