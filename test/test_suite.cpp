#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <filesystem>
#include <chrono>
#include <algorithm>

class CHTLTestSuite {
public:
    struct TestResult {
        std::string test_name;
        bool passed;
        std::string error_message;
        std::chrono::milliseconds duration;
    };
    
    struct TestCase {
        std::string name;
        std::string input_file;
        std::string expected_output;
        std::string description;
        bool should_fail;
    };
    
    CHTLTestSuite() {
        initializeTests();
    }
    
    void runAllTests() {
        std::cout << "🧪 Running CHTL Test Suite\n";
        std::cout << "========================\n\n";
        
        int total_tests = 0;
        int passed_tests = 0;
        int failed_tests = 0;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (const auto& test : test_cases_) {
            total_tests++;
            TestResult result = runTest(test);
            
            if (result.passed) {
                passed_tests++;
                std::cout << "✅ " << result.test_name << " (" << result.duration.count() << "ms)\n";
            } else {
                failed_tests++;
                std::cout << "❌ " << result.test_name << " (" << result.duration.count() << "ms)\n";
                std::cout << "   Error: " << result.error_message << "\n";
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n📊 Test Results\n";
        std::cout << "===============\n";
        std::cout << "Total Tests: " << total_tests << "\n";
        std::cout << "Passed: " << passed_tests << "\n";
        std::cout << "Failed: " << failed_tests << "\n";
        std::cout << "Success Rate: " << (passed_tests * 100.0 / total_tests) << "%\n";
        std::cout << "Total Duration: " << total_duration.count() << "ms\n";
        
        if (failed_tests == 0) {
            std::cout << "\n🎉 All tests passed!\n";
        } else {
            std::cout << "\n⚠️  Some tests failed. Please check the output above.\n";
        }
    }
    
    void runSpecificTest(const std::string& test_name) {
        auto it = std::find_if(test_cases_.begin(), test_cases_.end(),
                              [&test_name](const TestCase& test) {
                                  return test.name == test_name;
                              });
        
        if (it != test_cases_.end()) {
            TestResult result = runTest(*it);
            if (result.passed) {
                std::cout << "✅ " << result.test_name << " PASSED\n";
            } else {
                std::cout << "❌ " << result.test_name << " FAILED\n";
                std::cout << "Error: " << result.error_message << "\n";
            }
        } else {
            std::cout << "❌ Test not found: " << test_name << "\n";
        }
    }
    
    void generateTestReport(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create test report file: " << filename << "\n";
            return;
        }
        
        file << "CHTL Test Suite Report\n";
        file << "=====================\n\n";
        file << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        int total_tests = 0;
        int passed_tests = 0;
        int failed_tests = 0;
        
        for (const auto& test : test_cases_) {
            total_tests++;
            TestResult result = runTest(test);
            
            file << "Test: " << test.name << "\n";
            file << "Description: " << test.description << "\n";
            file << "Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
            file << "Duration: " << result.duration.count() << "ms\n";
            
            if (!result.passed) {
                file << "Error: " << result.error_message << "\n";
                failed_tests++;
            } else {
                passed_tests++;
            }
            
            file << "\n";
        }
        
        file << "Summary\n";
        file << "=======\n";
        file << "Total Tests: " << total_tests << "\n";
        file << "Passed: " << passed_tests << "\n";
        file << "Failed: " << failed_tests << "\n";
        file << "Success Rate: " << (passed_tests * 100.0 / total_tests) << "%\n";
        
        file.close();
        std::cout << "Test report generated: " << filename << "\n";
    }
    
private:
    std::vector<TestCase> test_cases_;
    
    void initializeTests() {
        // 基础语法测试
        test_cases_.push_back({
            "basic_template_style",
            "test/test_basic_template_style.chtl",
            "test/test_basic_template_style_expected.html",
            "Test basic template style syntax",
            false
        });
        
        test_cases_.push_back({
            "basic_template_element",
            "test/test_basic_template_element.chtl",
            "test/test_basic_template_element_expected.html",
            "Test basic template element syntax",
            false
        });
        
        test_cases_.push_back({
            "basic_template_var",
            "test/test_basic_template_var.chtl",
            "test/test_basic_template_var_expected.html",
            "Test basic template variable syntax",
            false
        });
        
        // 自定义元素测试
        test_cases_.push_back({
            "custom_style",
            "test/test_custom_style.chtl",
            "test/test_custom_style_expected.html",
            "Test custom style syntax",
            false
        });
        
        test_cases_.push_back({
            "custom_element",
            "test/test_custom_element.chtl",
            "test/test_custom_element_expected.html",
            "Test custom element syntax",
            false
        });
        
        // 导入和命名空间测试
        test_cases_.push_back({
            "import_namespace",
            "test/test_import_namespace.chtl",
            "test/test_import_namespace_expected.html",
            "Test import and namespace syntax",
            false
        });
        
        // 原始内容测试
        test_cases_.push_back({
            "origin_content",
            "test/test_origin_content.chtl",
            "test/test_origin_content_expected.html",
            "Test origin content embedding",
            false
        });
        
        // 配置测试
        test_cases_.push_back({
            "configuration",
            "test/test_configuration.chtl",
            "test/test_configuration_expected.html",
            "Test configuration syntax",
            false
        });
        
        // 模板继承测试
        test_cases_.push_back({
            "template_inheritance",
            "test/test_template_inheritance.chtl",
            "test/test_template_inheritance_expected.html",
            "Test template inheritance",
            false
        });
        
        // 模块系统测试
        test_cases_.push_back({
            "module_system",
            "test/test_module_system.chtl",
            "test/test_module_system_expected.html",
            "Test module system (CMOD/CJMOD)",
            false
        });
        
        // CHTL JS测试
        test_cases_.push_back({
            "chtl_js_basic",
            "test/test_chtl_js_basic.chtljs",
            "test/test_chtl_js_basic_expected.js",
            "Test basic CHTL JS syntax",
            false
        });
        
        test_cases_.push_back({
            "chtl_js_cjmod",
            "test/test_chtl_js_cjmod.chtljs",
            "test/test_chtl_js_cjmod_expected.js",
            "Test CHTL JS CJMOD syntax",
            false
        });
        
        test_cases_.push_back({
            "chtl_js_animation",
            "test/test_chtl_js_animation.chtljs",
            "test/test_chtl_js_animation_expected.js",
            "Test CHTL JS animation syntax",
            false
        });
        
        test_cases_.push_back({
            "chtl_js_route",
            "test/test_chtl_js_route.chtljs",
            "test/test_chtl_js_route_expected.js",
            "Test CHTL JS route syntax",
            false
        });
        
        // 错误处理测试
        test_cases_.push_back({
            "error_syntax",
            "test/test_error_syntax.chtl",
            "test/test_error_syntax_expected.html",
            "Test syntax error handling",
            true
        });
        
        test_cases_.push_back({
            "error_semantic",
            "test/test_error_semantic.chtl",
            "test/test_error_semantic_expected.html",
            "Test semantic error handling",
            true
        });
        
        // 性能测试
        test_cases_.push_back({
            "performance_large_file",
            "test/test_performance_large.chtl",
            "test/test_performance_large_expected.html",
            "Test performance with large file",
            false
        });
        
        // 边界情况测试
        test_cases_.push_back({
            "edge_cases",
            "test/test_edge_cases.chtl",
            "test/test_edge_cases_expected.html",
            "Test edge cases and boundary conditions",
            false
        });
    }
    
    TestResult runTest(const TestCase& test) {
        TestResult result;
        result.test_name = test.name;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            // 检查输入文件是否存在
            if (!std::filesystem::exists(test.input_file)) {
                result.passed = false;
                result.error_message = "Input file not found: " + test.input_file;
                return result;
            }
            
            // 运行编译器
            std::string command = getCompilerCommand(test.input_file);
            int exit_code = system(command.c_str());
            
            auto end_time = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            if (test.should_fail) {
                // 期望失败的测试
                result.passed = (exit_code != 0);
                if (!result.passed) {
                    result.error_message = "Expected test to fail but it succeeded";
                }
            } else {
                // 期望成功的测试
                result.passed = (exit_code == 0);
                if (!result.passed) {
                    result.error_message = "Compilation failed with exit code: " + std::to_string(exit_code);
                } else {
                    // 检查输出文件
                    std::string output_file = getOutputFile(test.input_file);
                    if (!std::filesystem::exists(output_file)) {
                        result.passed = false;
                        result.error_message = "Output file not generated: " + output_file;
                    } else {
                        // 比较输出文件
                        if (!compareFiles(output_file, test.expected_output)) {
                            result.passed = false;
                            result.error_message = "Output does not match expected result";
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.error_message = "Exception: " + std::string(e.what());
        }
        
        return result;
    }
    
    std::string getCompilerCommand(const std::string& input_file) {
        std::string ext = std::filesystem::path(input_file).extension();
        
        if (ext == ".chtljs") {
            return "./build/chtl_js_compiler \"" + input_file + "\" \"" + getOutputFile(input_file) + "\"";
        } else {
            return "./build/working_chtl_compiler \"" + input_file + "\" \"" + getOutputFile(input_file) + "\"";
        }
    }
    
    std::string getOutputFile(const std::string& input_file) {
        std::string ext = std::filesystem::path(input_file).extension();
        std::string basename = std::filesystem::path(input_file).stem();
        
        if (ext == ".chtljs") {
            return "test/output/" + basename + ".js";
        } else {
            return "test/output/" + basename + ".html";
        }
    }
    
    bool compareFiles(const std::string& file1, const std::string& file2) {
        // 简化的文件比较
        if (!std::filesystem::exists(file1) || !std::filesystem::exists(file2)) {
            return false;
        }
        
        std::ifstream f1(file1);
        std::ifstream f2(file2);
        
        std::string line1, line2;
        while (std::getline(f1, line1) && std::getline(f2, line2)) {
            if (line1 != line2) {
                return false;
            }
        }
        
        return f1.eof() && f2.eof();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
        return std::string(buffer);
    }
};

int main(int argc, char* argv[]) {
    CHTLTestSuite test_suite;
    
    if (argc > 1) {
        std::string test_name = argv[1];
        test_suite.runSpecificTest(test_name);
    } else {
        test_suite.runAllTests();
    }
    
    // 生成测试报告
    test_suite.generateTestReport("test_report.txt");
    
    return 0;
}