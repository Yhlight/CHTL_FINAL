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
    // In a real scenario, more robust normalization would be needed.
    return s;
}

TEST(ModuleSystemTest, ImportsAndUsesTemplateAndOrigin) {
    // 1. Create a temporary file to import
    std::string imported_filename = "module1.chtl";
    std::string imported_content = R"(
        [Template] @Element MyModuleTemplate {
            p {
                class: "module-text";
                text { "This content was imported from module1." }
            }
        }
    )";
    create_temp_file(imported_filename, imported_content);

    // 2. Define the main source content
    std::string main_source = R"(
        [Import] @Chtl from "module1.chtl";

        [Origin] @JavaScript MyScript {
            console.log("This is from an origin block.");
        }

        div {
            id: "main-container";
            @Element MyModuleTemplate;
        }
    )";

    // 3. Compile the main source
    CHTL::CHTLCompiler compiler;
    std::string html_output = compiler.Compile(main_source, std::filesystem::current_path().string());

    // 4. Check the output
    std::string expected_output =
        "<div id=\"main-container\"><p class=\"module-text\">This content was imported from module1.</p></div>"
        "<script>console.log(\"This is from an origin block.\");</script>";

    EXPECT_EQ(normalize_html(html_output), normalize_html(expected_output));

    // 5. Clean up the temporary file
    std::remove(imported_filename.c_str());
}
