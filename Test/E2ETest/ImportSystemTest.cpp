#include "gtest/gtest.h"
#include "CHTLCompiler/CHTLCompiler.h"
#include <filesystem>
#include <fstream>

// Helper function to create a temporary file
void create_temp_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
    out.close();
}

TEST(ImportSystemTest, ImportsAndUsesTemplate) {
    // 1. Create a temporary file to import
    std::string imported_filename = "imported_module.chtl";
    std::string imported_content = R"(
        [Template] @Element MyImportedTemplate {
            div {
                class: "imported";
                text { "Hello from imported file!" }
            }
        }
    )";
    create_temp_file(imported_filename, imported_content);

    // 2. Create the main source file content that imports the other file
    std::string main_source = R"(
        [Import] @Chtl from "imported_module.chtl";

        h1 { text { "Main File" } }
        @Element MyImportedTemplate;
    )";

    // 3. Compile the main source
    CHTL::CHTLCompiler compiler;
    std::string html_output = compiler.Compile(main_source, std::filesystem::current_path().string());

    // 4. Check the output
    std::string expected_output = "<h1>Main File</h1><div class=\"imported\">Hello from imported file!</div>";

    // Normalize whitespace for comparison
    html_output.erase(std::remove(html_output.begin(), html_output.end(), '\n'), html_output.end());
    html_output.erase(std::remove(html_output.begin(), html_output.end(), '\r'), html_output.end());

    EXPECT_EQ(html_output, expected_output);

    // 5. Clean up the temporary file
    std::remove(imported_filename.c_str());
}
