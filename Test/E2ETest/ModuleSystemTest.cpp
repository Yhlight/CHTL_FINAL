#include "gtest/gtest.h"
#include "CHTLCompiler/CHTLCompiler.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>

// Helper function to create a temporary file
void create_temp_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
    out.close();
}

// Helper to normalize HTML output for comparison
std::string normalize_html(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
    return s;
}

TEST(NamespaceSystemTest, HandlesNamespacesAndDefaultNamespaces) {
    // 1. Create module with an explicit namespace
    std::string ns_module_filename = "ns_module.chtl";
    std::string ns_module_content = R"(
        [Namespace] MySpace {
            [Template] @Element NamespacedTemplate {
                p { text { "Inside MySpace" } }
            }
        }
    )";
    create_temp_file(ns_module_filename, ns_module_content);

    // 2. Create module with no namespace (will get a default one)
    std::string default_ns_module_filename = "default_ns_module.chtl";
    std::string default_ns_module_content = R"(
        [Template] @Element DefaultTemplate {
            p { text { "Inside default_ns_module" } }
        }
    )";
    create_temp_file(default_ns_module_filename, default_ns_module_content);

    // 3. Create the main source file content that imports both
    std::string main_source = R"(
        [Import] @Chtl from "ns_module.chtl";
        [Import] @Chtl from "default_ns_module.chtl";

        div {
            @Element NamespacedTemplate from MySpace;
            @Element DefaultTemplate from default_ns_module;
        }
    )";

    // 4. Compile the main source
    CHTL::CHTLCompiler compiler;
    std::string html_output = compiler.Compile(main_source, std::filesystem::current_path().string());

    // 5. Check the output
    std::string expected_output = "<div><p>InsideMySpace</p><p>Insidedefault_ns_module</p></div>";

    EXPECT_EQ(normalize_html(html_output), normalize_html(expected_output));

    // 6. Clean up the temporary files
    std::remove(ns_module_filename.c_str());
    std::remove(default_ns_module_filename.c_str());
}
