#pragma once

#include <string>

// This function encapsulates the core compilation pipeline.
// It takes CHTL source code as input and returns the generated HTML.
std::string compile_source(const std::string& source);
