#pragma once

#include <string>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher() = default;

    // Compiles a single file and returns the final output (e.g., HTML or JS string)
    std::string compile(const std::string& entry_filepath);

private:
    std::string compileCHTL(const std::string& filepath, const std::string& source);
    std::string compileCHTL_JS(const std::string& filepath, const std::string& source);
    // Add stubs for CSS and JS if needed
};

} // namespace CHTL
