#pragma once

#include <string>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher() = default;

    // Compiles a single file and returns the final output (e.g., HTML or JS string)
    std::string compile(const std::string& entry_filepath, bool default_struct = false);

private:
    std::string compileCHTL(const std::string& filepath, const std::string& source, bool default_struct);
    std::string compileCHTL_JS(const std::string& filepath, const std::string& source);
    // Add stubs for CSS and JS if needed
};

} // namespace CHTL
